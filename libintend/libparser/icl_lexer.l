/***************************************************************************
 *                                                                         *
 *   Intend C - Embeddable Scripting Language                              *
 *                                                                         *
 *   Copyright (C) 2008 by Pedro Reis Colaço <info@intendc.org>            *
 *   http://www.intendc.org                                                *
 *                                                                         *
 *   LICENSE INFORMATION:                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   ACKNOWLEDGEMENTS:                                                     *
 *                                                                         *
 *   This project was based on the work of Pascal Schmidt in project       *
 *   Arena. See http://www.minimalinux.org/arena/ for more information.    *
 *                                                                         *
 ***************************************************************************/

/*
 *
 * flex Implementation: lexer
 *
 * Description: Intend C Lexical definitions
 *
 */

/*
 *
 * Use options
 *
 * 	%option prefix="foo"
 * 	%option outfile="lex.yy.c"
 *
 * to create multiple flex scanner in one project.
 *
 */

%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "icl_grammar.h"

/*
 * The type for yyextra data
 */
#define YY_EXTRA_TYPE intend_state *

/*
 * Helper macros for position updates
 */
#define global_state yyextra
#define parser_state ((parser_data *)yyextra->parser)

#define COLS global_state->source_col += yyleng
#define LINES global_state->source_line++; global_state->source_col = 0

/*
 * Helper macro for nested comment block handling
 */
#define BLOCK if (parser_state->in_block == 0)

/*
 * We need to have a prototype here to avoid warnings
 */
void icl_set_lval (YYSTYPE * yylval_param ,yyscan_t yyscanner );
YYSTYPE * icl_get_lval (yyscan_t yyscanner );

/*
 * Record filename for eventual deallocation
 */
static void record_file(intend_state *state, char *path)
{
    parser_data *p = (parser_data *)state->parser;
    char **names;
    int depth = p->include_all + 1;

    names = oom(realloc(p->include_all_files, depth * sizeof(char *)));
    names[p->include_all] = path;

    p->include_all_files = names;
    ++p->include_all;
}

/*
 * Enter new include file
 */
void parser_enter_include(intend_state *state, char *path)
{
    parser_data *p = (parser_data *)state->parser;
    char **files;
    FILE **fps;
    int *lines;
    int *cols;
    YY_BUFFER_STATE *buf_state;
    FILE *inc;
    int depth = p->include_depth + 1;

    inc = (*state->get_source)(state, path);
    if (!inc) {
        icl_error(state, "could not open include source");
        return;
    }

    record_file(state, path);

    files = oom(realloc(p->include_files, depth * sizeof(char *)));
    fps   = oom(realloc(p->include_fps, depth * sizeof(FILE *)));
    lines = oom(realloc(p->include_lines, depth * sizeof(int)));
    cols  = oom(realloc(p->include_cols, depth * sizeof(int)));

    files[p->include_depth] = state->source_file;
    fps[p->include_depth]   = inc;
    lines[p->include_depth] = state->source_line;
    cols[p->include_depth]  = state->source_col;

    p->include_files = files;
    p->include_fps   = fps;
    p->include_lines = lines;
    p->include_cols  = cols;
    p->include_depth++;

    state->source_file = path;
    state->source_line = 1;
    state->source_col = 0;

    icl_push_buffer_state(icl__create_buffer(inc, YY_BUF_SIZE, p->scanner), p->scanner);
}

/*
 * Leave include file
 */
static int leave_include(intend_state *state)
{
    parser_data *p = (parser_data *)state->parser;

    if (p->include_depth == 0) {
        free(p->include_lines);
        free(p->include_cols);
        free(p->include_files);
        free(p->include_fps);
        p->include_lines = p->include_cols = NULL;
        p->include_files = NULL;

        icl_pop_buffer_state(p->scanner);
        return 0;
    }

    p->include_depth--;
    state->source_line = p->include_lines[p->include_depth];
    state->source_col  = p->include_cols[p->include_depth];
    state->source_file = p->include_files[p->include_depth];
    fclose(p->include_fps[p->include_depth]);

    icl_pop_buffer_state(p->scanner);

    return 1;
}

/*
 * Clean up memory used by include file names
 */
void parser_include_teardown(intend_state *state)
{
    parser_data *p;
    int i;

    sanity(state);

    if (!state->parser)
        return;

    p = (parser_data *)state->parser;

    for (i = 0; i < p->include_all; i++) {
        free(p->include_all_files[i]);
    }
    free(p->include_all_files);
}

/*
 * Adjust line and column for string literals
 */
static void literal(intend_state *state, char *text, int len)
{
    int i;

    for (i = 0; i < len; ++i) {
        if (text[i] == '\n') {
            ++state->source_line;
            state->source_col = 0;
        } else {
            ++state->source_col;
        }
    }
}

/*
 * Copy arbitrary string to token buffer
 *
 * This function does a string copy of the given token into the
 * parser's semantic union.
 */
static void copy_string(intend_state *state, const char *from, int len)
{
    parser_data *p = (parser_data *)state->parser;
    char *copy;
    YYSTYPE *lval;

    copy = malloc(len + 1);
    if (!copy) {
        p->parser_error++;
        return;
    }
    memcpy(copy, from, len);
    copy[len] = 0;
    lval = icl_get_lval(p->scanner);
    lval->string = copy;
    icl_set_lval(lval, p->scanner);
}

static int input (yyscan_t yyscanner );

/*
 * Handle CPP style comment
 * Constume everything until the newline or EOF
 */
static void line_comment(intend_state *state)
{
    parser_data *p = (parser_data *)state->parser;
    register int c;

    while ((c = input(p->scanner)) != '\n' && c != EOF)
        state->source_col++;

    if (c == '\n') {
        state->source_line++;
        state->source_col = 0;
    }
}

%}

%option reentrant

%option prefix="icl_"

/*
 * Outfile needed to allow automake to post-process the file and
 * rename it to lexer.c (these AM guys are crazy)
 */
%option outfile="lex.yy.c"

%option bison-bridge

%option noyywrap


ID [_a-zA-Z][_a-zA-Z0-9]*
INTVAL [1-9][0-9]*
OCTVAL 0[0-7]*
HEXVAL 0x[0-9a-zA-Z]+
FLOATVAL ([0-9]*)?\.[0-9]+([eE][+\-][0-9]*)?
STRINGVAL \"([^\\\"]|\\.)*\"
STRINGLIT \'([^\\\']|\\.)*\'
WHITE [ \r\t]

%%

{WHITE}*        { COLS; /* eat whitespace */ }
"#"             { line_comment(global_state); /* eat trailing shell line comment */ }
"//"            { line_comment(global_state); /* eat trailing C++ line comment */ }
"\n"            { LINES; }
"/\*"           { COLS; parser_state->in_block++; }
"\*/"           { COLS; parser_state->in_block--; }
"include"       { COLS; BLOCK return KW_INC; }
"use"           { COLS; BLOCK return KW_USE; }
"if"            { COLS; BLOCK return KW_IF; }
"else"          { COLS; BLOCK return KW_ELSE; }
"while"         { COLS; BLOCK return KW_WHILE; }
"do"            { COLS; BLOCK return KW_DO; }
"return"        { COLS; BLOCK return KW_RETURN; }
"for"           { COLS; BLOCK return KW_FOR; }
"continue"      { COLS; BLOCK return KW_CONTINUE; }
"break"         { COLS; BLOCK return KW_BREAK; }
"switch"        { COLS; BLOCK return KW_SWITCH; }
"case"          { COLS; BLOCK return KW_CASE; }
"default"       { COLS; BLOCK return KW_DEFAULT; }
"forced"        { COLS; BLOCK return KW_FORCE; }
"try"           { COLS; BLOCK return KW_TRY; }
"throw"         { COLS; BLOCK return KW_THROW; }
"catch"         { COLS; BLOCK return KW_CATCH; }
"class"         { COLS; BLOCK return KW_CLASS; }
"extends"       { COLS; BLOCK return KW_EXTENDS; }
"new"           { COLS; BLOCK return KW_NEW; }
"__FILE__"      { COLS; BLOCK return KW_FILE; }
"__LINE__"      { COLS; BLOCK return KW_LINE; }
"NULL"          { COLS; BLOCK return KW_NULL; }
"::"            { COLS; BLOCK return KW_STATIC; }
"\(void\)"      { COLS; BLOCK return CAST_VOID; }
"\(bool\)"      { COLS; BLOCK return CAST_BOOL; }
"\(int\)"       { COLS; BLOCK return CAST_INT; }
"\(float\)"     { COLS; BLOCK return CAST_FLOAT; }
"\(string\)"    { COLS; BLOCK return CAST_STRING; }
"\(array\)"     { COLS; BLOCK return CAST_ARRAY; }
"\(struct\)"    { COLS; BLOCK return CAST_STRUCT; }
"void"          { COLS; BLOCK return TYPE_VOID; }
"bool"          { COLS; BLOCK return TYPE_BOOL; }
"int"           { COLS; BLOCK return TYPE_INT; }
"float"         { COLS; BLOCK return TYPE_FLOAT; }
"string"        { COLS; BLOCK return TYPE_STRING; }
"array"         { COLS; BLOCK return TYPE_ARRAY; }
"struct"        { COLS; BLOCK return TYPE_STRUCT; }
"fn"            { COLS; BLOCK return TYPE_FN; }
"resource"      { COLS; BLOCK return TYPE_RES; }
"mixed"         { COLS; BLOCK return TYPE_MIXED; }
"=="            { COLS; BLOCK return OP_EQUAL; }
"!="            { COLS; BLOCK return OP_NOT_EQUAL; }
"<="            { COLS; BLOCK return OP_SEQ; }
">="            { COLS; BLOCK return OP_LEQ; }
"<"             { COLS; BLOCK return OP_SMALLER; }
">"             { COLS; BLOCK return OP_LARGER; }
"\+\+"          { COLS; BLOCK return OP_PLUSPLUS; }
"\-\-"          { COLS; BLOCK return OP_MINUSMINUS; }
"&&"            { COLS; BLOCK return OP_BOOL_AND; }
"\|\|"          { COLS; BLOCK return OP_BOOL_OR; }
"\*\*"          { COLS; BLOCK return OP_POW; }
"\+"            { COLS; BLOCK return OP_PLUS; }
"\-"            { COLS; BLOCK return OP_MINUS; }
"\*"            { COLS; BLOCK return OP_MUL; }
"/"             { COLS; BLOCK return OP_DIV; }
"%"             { COLS; BLOCK return OP_MOD; }
"&"             { COLS; BLOCK return OP_AND; }
"\|"            { COLS; BLOCK return OP_OR; }
"\^"            { COLS; BLOCK return OP_XOR; }
"<<"            { COLS; BLOCK return OP_LSHIFT; }
">>"            { COLS; BLOCK return OP_RSHIFT; }
"\!"            { COLS; BLOCK return OP_NOT; }
"~"             { COLS; BLOCK return OP_NEG; }
"="             { COLS; BLOCK return OP_ASSIGN; }
"\+="           { COLS; BLOCK return OP_APLUS; }
"\-="           { COLS; BLOCK return OP_AMINUS; }
"\*="           { COLS; BLOCK return OP_AMUL; }
"/="            { COLS; BLOCK return OP_ADIV; }
"&="            { COLS; BLOCK return OP_AAND; }
"\|="           { COLS; BLOCK return OP_AOR; }
"\^="           { COLS; BLOCK return OP_AXOR; }
"<<="           { COLS; BLOCK return OP_ALSHIFT; }
">>="           { COLS; BLOCK return OP_ARSHIFT; }
"false"         { COLS; BLOCK copy_string(global_state, yytext, yyleng); BLOCK return CONST_BOOL; }
"true"          { COLS; BLOCK copy_string(global_state, yytext, yyleng); BLOCK return CONST_BOOL; }
{INTVAL}        { COLS; BLOCK copy_string(global_state, yytext, yyleng); BLOCK return CONST_INT; }
{OCTVAL}        { COLS; BLOCK copy_string(global_state, yytext, yyleng); BLOCK return CONST_INT; }
{HEXVAL}        { COLS; BLOCK copy_string(global_state, yytext, yyleng); BLOCK return CONST_INT; }
{FLOATVAL}      { COLS; BLOCK copy_string(global_state, yytext, yyleng); BLOCK return CONST_FLOAT; }
{STRINGVAL}     { literal(global_state, yytext, yyleng); BLOCK copy_string(global_state, yytext+1, yyleng-2); BLOCK return CONST_STRING; }
{STRINGLIT}     { literal(global_state, yytext, yyleng); BLOCK copy_string(global_state, yytext+1, yyleng-2); BLOCK return CONST_STRING; }
{ID}            { COLS; BLOCK copy_string(global_state, yytext, yyleng); BLOCK return ID; }
.               { COLS; BLOCK return *yytext; }
<<EOF>>         { if (leave_include(global_state) == 0) return 0; }
