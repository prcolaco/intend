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
 * bison/yacc Implementation: grammar
 *
 * Description: Intend C Language Grammar
 *
 */

/*
 *
 * At Makefile.am insert these lines before 'bin_SCRIPTS' command:
 *
 *    BUILT_SOURCES = grammar.h
 *    AM_YFLAGS = -d
 *
 * Use options
 *
 *    %name-prefix="foo"
 *    %yacc
 *
 * to create multiple parsers in one project.
 *
 */

%pure-parser

%name-prefix="icl_"

%defines

%error-verbose

%parse-param { intend_state *state }
%lex-param   { void *scanner       }

%union {
    char *string;
    int unused;
}

%token <string> CONST_BOOL
%token <string> CONST_INT
%token <string> CONST_FLOAT
%token <string> CONST_STRING

%token <unused> KW_INC
%token <unused> KW_USE
%token <unused> KW_IF
%token <unused> KW_ELSE
%token <unused> KW_WHILE
%token <unused> KW_DO
%token <unused> KW_RETURN
%token <unused> KW_FOR
%token <unused> KW_CONTINUE
%token <unused> KW_BREAK
%token <unused> KW_SWITCH
%token <unused> KW_CASE
%token <unused> KW_DEFAULT
%token <unused> KW_FORCE
%token <unused> KW_TRY
%token <unused> KW_THROW
%token <unused> KW_CATCH
%token <unused> KW_CLASS
%token <unused> KW_EXTENDS
%token <unused> KW_NEW
%token <unused> KW_STATIC
%token <unused> KW_FILE
%token <unused> KW_LINE
%token <unused> KW_NULL

%token <unused> TYPE_VOID
%token <unused> TYPE_BOOL
%token <unused> TYPE_INT
%token <unused> TYPE_FLOAT
%token <unused> TYPE_STRING
%token <unused> TYPE_ARRAY
%token <unused> TYPE_STRUCT
%token <unused> TYPE_FN
%token <unsued> TYPE_RES
%token <unused> TYPE_MIXED

%right <unused> OP_ASSIGN OP_APLUS OP_AMINUS OP_AMUL OP_ADIV OP_AAND OP_AOR OP_AXOR OP_ALSHIFT OP_ARSHIFT
%nonassoc <unused> '?' ':'
%right <unused> OP_BOOL_OR
%right <unused> OP_BOOL_AND
%right <unused> OP_NOT
%nonassoc <unused> OP_EQUAL OP_NOT_EQUAL OP_SEQ OP_LEQ OP_SMALLER OP_LARGER
%left <unused> OP_AND OP_OR OP_XOR
%left <unused> OP_PLUS OP_MINUS 
%left <unused> OP_MUL OP_DIV OP_MOD
%right <unused> OP_POW
%left <unused> OP_LSHIFT OP_RSHIFT
%left <unused> UNARY_MINUS OP_NEG
%left <unused> OP_PLUSPLUS OP_MINUSMINUS
%right <unused> CAST_VOID CAST_BOOL CAST_INT CAST_UINT CAST_FLOAT CAST_STRING CAST_ARRAY CAST_STRUCT

%token <string> ID

%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

/*
 * Print parser error message
 */
void icl_error(intend_state *state, char *msg)
{
    parser_data *p = (parser_data *)state->parser;

    fatal(state, msg);
    p->parser_error++;
}

/*
 * Constant to translate the state to scanner context
 * before calling lex
 */
#define scanner ((parser_data *)state->parser)->scanner

%}

%%

program: /* empty */
    | fstatements
    ;

fstatements: fblock
    | fstatements fblock
    ;

fblock: fundef
    | classdef
    | block
    ;

statements: block
    | statements block
    ;

block:  statement ';'
    | KW_INC CONST_STRING ';' { parser_enter_include(state, $2); }
    | control
    | '{' { stmt_begin_block(state); } statements '}' { stmt_end_block(state); }
    | '{' '}' { stmt_begin_block(state); stmt_end_nop(state); stmt_end_block(state); }
    | ';' { stmt_end_nop(state); } 
    ;

control: KW_IF { stmt_begin_if(state); } '(' expr ')' block else { stmt_end_if(state); }
    | KW_WHILE { stmt_begin_while(state); } '(' expr ')' block { stmt_end_while(state); }
    | KW_DO { stmt_begin_do(state); } block KW_WHILE '(' expr ')' { stmt_end_do(state); }
    | KW_FOR { stmt_begin_for(state); } '(' nullexpr ';' nullexpr ';' nullexpr ')' block { stmt_end_for(state); }
    | KW_SWITCH { stmt_begin_switch(state); } '(' expr ')' '{' cases '}' { stmt_end_switch(state); }
    | KW_TRY { stmt_begin_try(state); } block KW_CATCH '(' ID ')' block { stmt_end_try(state, $6); }
    ;

else:   /* empty */
    | KW_ELSE block         { /* nop */ }
    ;

cases: { stmt_begin_case(state) ; } case  { /* nop */ }
    | cases { stmt_begin_case(state) ; } case { /* nop */ }
    ;

case:   KW_CASE expr ':' statements { stmt_end_case(state,  1); }
        | KW_CASE expr ':' { stmt_end_nop(state); stmt_end_case(state,  1); }
    | KW_CASE expr ':' statements KW_BREAK ';' { stmt_end_case(state,  0); }
    | KW_DEFAULT ':' statements { stmt_end_default(state); }
    | KW_DEFAULT ':' { stmt_end_nop(state); stmt_end_default(state); }
    ;

nullexpr: expr              { /* nop */ }
    |               { expr_end_true(state); }
    ;

statement: expr             { stmt_end_expr(state); }
    | KW_CONTINUE           { stmt_end_continue(state); }
    | KW_BREAK          { stmt_end_break(state); }
    | KW_RETURN         { stmt_end_return(state); }
    | KW_RETURN expr        { stmt_end_return_value(state); }
    | KW_THROW expr         { stmt_end_throw(state); }
    ;

expr:   ref                 { /* nop */ }
    | moduse            { /* nop */ }
    | indexable         { /* nop */ }
    | indexable indexes     { /* nop */ }
    | cast              { /* nop */ }
    | const             { /* nop */ }
    | infix             { /* nop */ }
    | postfix           { /* nop */ }
    | prefix            { /* nop */ }
    | assign            { /* nop */ }
    | lambda            { /* nop */ }
    | expr '?' expr ':' expr    { expr_end_if(state); }
    | KW_FILE           { expr_end_file(state); }
    | KW_LINE           { expr_end_line(state); }
    ;

indexable: '(' expr ')'         { /* nop */ }
    | call              { /* nop */ }
    | arrinit           { /* nop */ }
    | structinit        { /* nop */ }
    | cons              { /* nop */ }
    | static            { /* nop */ }
    | method            { /* nop */ }
    ;

indexes: index
    | indexes index
    ;
    
index: '[' expr ']'         { expr_end_first(state); expr_end_index(state); }
    | '.' ID            { expr_end_field(state, $2); expr_end_first(state); expr_end_index(state); }
    | '.' ID '(' ')'        { expr_end_method_void(state, $2); }
    | '.' ID '(' arglist ')'    { expr_end_method(state, $2); }
    ;

lambda: '\\' { expr_begin_lambda(state); } '(' argdefs ')' '{' statements '}' { expr_end_lambda(state); }
    ;

moduse: KW_USE modlist      { expr_end_use(state); }
    ;

modlist: modid            { expr_end_first(state); }
    | modlist ',' modid       { expr_end_next(state); }
    ;

modid: ID            { expr_end_const_string(state, $1); }
    ;

call:   ID '(' ')'          { expr_end_call_void(state, $1, 0); }
    | ID '(' arglist ')'        { expr_end_call(state, $1, 0); }
    ;

cons:   KW_NEW ID '(' ')'       { expr_end_call_void(state, $2, 1); }
    | KW_NEW ID '(' arglist ')' { expr_end_call(state, $2, 1); }
    ;

arrinit: KW_NEW TYPE_ARRAY '(' ')'         { expr_end_typeinit_void(state, 1); }
    |    KW_NEW TYPE_ARRAY '(' arrlist ')' { expr_end_typeinit(state, 1); }
    ;

arrlist: arrarg                 { expr_end_first(state); expr_end_next(state); }
    |    arrlist ',' arrarg     { expr_end_next(state); expr_end_next(state); }
    ;

arrarg: expr { expr_end_keyarray_single(state); }
    |   expr OP_ASSIGN expr     { /* nop */ }
    ;

structinit: KW_NEW TYPE_STRUCT '(' ')'            { expr_end_typeinit_void(state, 2); }
    |       KW_NEW TYPE_STRUCT '(' structlist ')' { expr_end_typeinit(state, 2); }
    ;

structlist: structarg                    { expr_end_first(state); expr_end_next(state); }
    |       structlist ',' structarg     { expr_end_next(state); expr_end_next(state); }
    ;

structarg: ID { expr_end_const_string(state, $1); expr_end_const_void(state); }
    |      ID { expr_end_const_string(state, $1); } OP_ASSIGN expr     { /* nop */ }
    ;

static: ID KW_STATIC ID '(' ')'     { expr_end_static_void(state, $1, $3); }
    | ID KW_STATIC ID '(' arglist ')' { expr_end_static(state, $1, $3); }
    ;

method: ID refexp '.' ID '(' ')' { expr_end_ref_array(state, $1); expr_end_method_void(state, $4); }
    | ID refexp '.' ID '(' { expr_end_ref_array(state, $1); } arglist ')' { expr_end_method(state, $4); }
    | ID '.' ID '(' ')' { expr_end_ref(state, $1); expr_end_method_void(state, $3); }
    | ID '.' ID '(' { expr_end_ref(state, $1); } arglist ')' { expr_end_method(state, $3); }
    ;

ref:    ID              { expr_end_ref(state, $1); }
    | ID refexp         { expr_end_ref_array(state, $1); }
    | ID KW_STATIC ID       { expr_end_static_ref(state, $1, $3); }
    ;

refexp: '[' expr ']'            { expr_end_first(state); }
    | '.' ID            { expr_end_field(state, $2); expr_end_first(state); }
    | refexp '[' expr ']'       { expr_end_next(state); }
    | refexp '.' ID         { expr_end_field(state, $3); expr_end_next(state); }
    ;

assign: ID OP_ASSIGN expr       { expr_end_assign(state, $1); }
    | ID OP_APLUS expr      { expr_end_op_assign(state, $1, OPTYPE_PLUS); }
    | ID OP_AMINUS expr     { expr_end_op_assign(state, $1, OPTYPE_MINUS); }
    | ID OP_AMUL expr       { expr_end_op_assign(state, $1, OPTYPE_MUL); }
    | ID OP_ADIV expr       { expr_end_op_assign(state, $1, OPTYPE_DIV); }
    | ID OP_AAND expr       { expr_end_op_assign(state, $1, OPTYPE_AND); }
    | ID OP_AOR expr        { expr_end_op_assign(state, $1, OPTYPE_OR); }
    | ID OP_AXOR expr       { expr_end_op_assign(state, $1, OPTYPE_XOR); }
    | ID OP_ALSHIFT expr        { expr_end_op_assign(state, $1, OPTYPE_LSHIFT); }
    | ID OP_ARSHIFT expr        { expr_end_op_assign(state, $1, OPTYPE_RSHIFT); }
    | ID refexp OP_ASSIGN expr  { expr_end_assign_array(state, $1); }
    | ID refexp OP_APLUS expr   { expr_end_op_assign_array(state, $1, OPTYPE_PLUS); }
    | ID refexp OP_AMINUS expr  { expr_end_op_assign_array(state, $1, OPTYPE_MINUS); }
    | ID refexp OP_AMUL expr    { expr_end_op_assign_array(state, $1, OPTYPE_MUL); }
    | ID refexp OP_ADIV expr    { expr_end_op_assign_array(state, $1, OPTYPE_DIV); }
    | ID refexp OP_AAND expr    { expr_end_op_assign_array(state, $1, OPTYPE_AND); }
    | ID refexp OP_AOR expr { expr_end_op_assign_array(state, $1, OPTYPE_OR); }
    | ID refexp OP_AXOR expr    { expr_end_op_assign_array(state, $1, OPTYPE_XOR); }
    | ID refexp OP_ALSHIFT expr { expr_end_op_assign_array(state, $1, OPTYPE_LSHIFT); }
    | ID refexp OP_ARSHIFT expr { expr_end_op_assign_array(state, $1, OPTYPE_RSHIFT); }
    ;

prefix: OP_NOT expr         { expr_end_prefix(state, OPTYPE_NOT); }
    | OP_MINUS expr %prec UNARY_MINUS { expr_end_prefix(state, OPTYPE_MINUS); }
    | OP_PLUSPLUS expr      { expr_end_prefix(state, OPTYPE_PREINC); }
    | OP_MINUSMINUS expr        { expr_end_prefix(state, OPTYPE_PREDEC); }
    | OP_NEG expr           { expr_end_prefix(state, OPTYPE_NEG); }
    ;

infix:  expr OP_PLUS expr       { expr_end_infix(state, OPTYPE_PLUS); }
    | expr OP_MINUS expr        { expr_end_infix(state, OPTYPE_MINUS); }
    | expr OP_MUL expr      { expr_end_infix(state, OPTYPE_MUL); }
    | expr OP_DIV expr      { expr_end_infix(state, OPTYPE_DIV); }
    | expr OP_MOD expr      { expr_end_infix(state, OPTYPE_MOD); }
    | expr OP_POW expr      { expr_end_infix(state, OPTYPE_POW); }
    | expr OP_BOOL_AND expr     { expr_end_infix(state, OPTYPE_BOOL_AND); }
    | expr OP_BOOL_OR expr      { expr_end_infix(state, OPTYPE_BOOL_OR); }
    | expr OP_AND expr      { expr_end_infix(state, OPTYPE_AND); }
    | expr OP_OR expr       { expr_end_infix(state, OPTYPE_OR); }
    | expr OP_XOR expr      { expr_end_infix(state, OPTYPE_XOR); }
    | expr OP_LSHIFT expr           { expr_end_infix(state, OPTYPE_LSHIFT); }
    | expr OP_RSHIFT expr           { expr_end_infix(state, OPTYPE_RSHIFT); }
    | expr OP_EQUAL expr        { expr_end_infix(state, OPTYPE_EQUAL); }
    | expr OP_NOT_EQUAL expr    { expr_end_infix(state, OPTYPE_NOT_EQUAL); }
    | expr OP_SEQ expr      { expr_end_infix(state, OPTYPE_SEQ); }
    | expr OP_LEQ expr      { expr_end_infix(state, OPTYPE_LEQ); }
    | expr OP_SMALLER expr      { expr_end_infix(state, OPTYPE_SMALLER); }
    | expr OP_LARGER expr       { expr_end_infix(state, OPTYPE_LARGER); }
    ;

postfix: expr OP_PLUSPLUS       { expr_end_postfix(state, OPTYPE_POSTINC); }
    | expr OP_MINUSMINUS        { expr_end_postfix(state, OPTYPE_POSTDEC); }
    ;

arglist: argexpr            { expr_end_first(state); }
    | arglist ',' argexpr       { expr_end_next(state); }
    ;

argexpr: OP_AND expr            { expr_end_pass_ref(state); }
    | expr              { /* nop */ }
    ;

const:  CONST_BOOL      { expr_end_const_bool(state, $1); }
    | CONST_INT     { expr_end_const_int(state, $1); }
    | CONST_FLOAT       { expr_end_const_float(state, $1); }
    | conststring      { /* nop */ }
    | KW_NULL       { expr_end_const_void(state); }
    | '(' ')'       { expr_end_const_void(state); }
    ;

conststring: CONST_STRING           { expr_end_const_string(state, $1); }
    | conststring CONST_STRING      { expr_end_const_string(state, $2); expr_end_infix(state, OPTYPE_PLUS); }
    ;

cast:   CAST_VOID expr      { expr_end_cast(state, "void"); }
    | CAST_BOOL expr    { expr_end_cast(state, "bool"); }
    | CAST_INT expr     { expr_end_cast(state, "int"); }
    | CAST_UINT expr    { expr_end_cast(state, "uint"); }
    | CAST_FLOAT expr   { expr_end_cast(state, "float"); }
    | CAST_STRING expr  { expr_end_cast(state, "string"); }
    | CAST_ARRAY expr   { expr_end_cast(state, "array"); }
    | CAST_STRUCT expr  { expr_end_cast(state, "struct"); }
    ;

fundef: { stmt_begin_func(state); } rettype ID '(' argdefs ')' block { stmt_end_func(state, $3); }
    ;

rettype: TYPE_VOID      { stmt_end_rettype(state, 'v'); }
    | TYPE_BOOL     { stmt_end_rettype(state, 'b'); }
    | TYPE_INT      { stmt_end_rettype(state, 'i'); }
    | TYPE_FLOAT        { stmt_end_rettype(state, 'f'); }
    | TYPE_STRING       { stmt_end_rettype(state, 's'); }
    | TYPE_ARRAY        { stmt_end_rettype(state, 'a'); }
    | TYPE_STRUCT       { stmt_end_rettype(state, 'c'); }
    | TYPE_FN       { stmt_end_rettype(state, 'p'); }
    | TYPE_RES      { stmt_end_rettype(state, 'r'); }
    | TYPE_MIXED        { stmt_end_rettype(state, '?'); }
    | KW_FORCE TYPE_VOID    { stmt_end_rettype(state, 'V'); }
    | KW_FORCE TYPE_BOOL    { stmt_end_rettype(state, 'B'); }
    | KW_FORCE TYPE_INT { stmt_end_rettype(state, 'I'); }
    | KW_FORCE TYPE_FLOAT   { stmt_end_rettype(state, 'F'); }
    | KW_FORCE TYPE_STRING  { stmt_end_rettype(state, 'S'); }
    | KW_FORCE TYPE_ARRAY   { stmt_end_rettype(state, 'A'); }
    | KW_FORCE TYPE_STRUCT  { stmt_end_rettype(state, 'C'); }
    | KW_FORCE TYPE_FN  { stmt_end_rettype(state, 'P'); }
    | KW_FORCE TYPE_RES { stmt_end_rettype(state, 'R'); }
    | KW_FORCE TYPE_MIXED   { stmt_end_rettype(state, '?'); }
    ;

argdefs: argdef         { /* nop */ }
    | argdefs ',' argdef    { /* nop */ }
    |                       { /* nop */ }
    ;

argdef: TYPE_VOID ID          { stmt_end_arg(state, 'v', $2); }
    | TYPE_BOOL ID        { stmt_end_arg(state, 'b', $2); }
    | TYPE_INT ID         { stmt_end_arg(state, 'i', $2); }
    | TYPE_FLOAT ID       { stmt_end_arg(state, 'f', $2); }
    | TYPE_STRING ID      { stmt_end_arg(state, 's', $2); }
    | TYPE_ARRAY ID       { stmt_end_arg(state, 'a', $2); }
    | TYPE_STRUCT ID      { stmt_end_arg(state, 'c', $2); }
    | TYPE_FN ID          { stmt_end_arg(state, 'p', $2); }
    | TYPE_RES ID             { stmt_end_arg(state, 'r', $2); }
    | TYPE_MIXED ID       { stmt_end_arg(state, '?', $2); }
    | KW_FORCE TYPE_VOID ID   { stmt_end_arg(state, 'V', $3); }
    | KW_FORCE TYPE_BOOL ID   { stmt_end_arg(state, 'B', $3); }
    | KW_FORCE TYPE_INT ID    { stmt_end_arg(state, 'I', $3); }
    | KW_FORCE TYPE_FLOAT ID  { stmt_end_arg(state, 'F', $3); }
    | KW_FORCE TYPE_STRING ID { stmt_end_arg(state, 'S', $3); }
    | KW_FORCE TYPE_ARRAY ID  { stmt_end_arg(state, 'A', $3); }
    | KW_FORCE TYPE_STRUCT ID { stmt_end_arg(state, 'C', $3); }
    | KW_FORCE TYPE_FN ID     { stmt_end_arg(state, 'P', $3); }
    | KW_FORCE TYPE_RES ID    { stmt_end_arg(state, 'R', $3); }
    | KW_FORCE TYPE_MIXED ID  { stmt_end_arg(state, '?', $3); }
    | ID              { stmt_end_arg(state, '?', $1); }
    ;

classdef: KW_CLASS { stmt_begin_class(state); } ID extends '{' classdecls '}' { stmt_end_class(state, $3); }
    ;

extends: KW_EXTENDS ID          { stmt_end_extends(state, $2); }     /* Java style */
    | ':' ID                    { stmt_end_extends(state, $2); }     /* C++ style */
    |                           { stmt_end_extends(state, NULL); }   /* No base class */
    ;

classdecls: classdecl
    | classdecls classdecl
    ;
    
classdecl: fundef            { /* nop */ }
    | ID ';'            { expr_end_const_void(state); expr_end_assign(state, $1); stmt_end_expr(state); }
    | ID OP_ASSIGN expr ';'     { expr_end_assign(state, $1); stmt_end_expr(state); }
    ;

%%
