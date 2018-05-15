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
 * Intend C Expression dumping
 */

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

/*
 * Determine operator precedence
 */
static int expr_prec(expr *ex, int *assoc)
{
    int res = 0;

    if (ex->type == EXPR_ASSIGN || ex->type == EXPR_ASSIGN_ARRAY) {
        *assoc = 1;
        return 0;
    }

    if (ex->type == EXPR_IF) {
        *assoc = 0;
        return 1;
    }

    if (ex->type == EXPR_PREFIX && ex->op == OPTYPE_MINUS) {
        *assoc = -1;
        return 11;
    }

    if (ex->type == EXPR_CAST) {
        *assoc = 1;
        return 13;
    }

    if (ex->type != EXPR_PREFIX &&
            ex->type != EXPR_INFIX  &&
            ex->type != EXPR_POSTFIX) {
        *assoc = 0;
        return 255;
    }

    switch (ex->op) {
        case OPTYPE_BOOL_OR:
            *assoc = 1;
            res = 2;
            break;
        case OPTYPE_BOOL_AND:
            *assoc = 1;
            res = 3;
            break;
        case OPTYPE_NOT:
            *assoc = 1;
            res = 4;
            break;
        case OPTYPE_EQUAL:
        case OPTYPE_NOT_EQUAL:
        case OPTYPE_SEQ:
        case OPTYPE_LEQ:
        case OPTYPE_SMALLER:
        case OPTYPE_LARGER:
            *assoc = 0;
            res = 5;
            break;
        case OPTYPE_AND:
        case OPTYPE_OR:
        case OPTYPE_XOR:
            *assoc = -1;
            res = 6;
            break;
        case OPTYPE_PLUS:
        case OPTYPE_MINUS:
            *assoc = -1;
            res = 7;
            break;
        case OPTYPE_MUL:
        case OPTYPE_DIV:
        case OPTYPE_MOD:
            *assoc = -1;
            res = 8;
            break;
        case OPTYPE_POW:
            *assoc = 1;
            res = 9;
            break;
        case OPTYPE_LSHIFT:
        case OPTYPE_RSHIFT:
            *assoc = -1;
            res = 10;
            break;
        case OPTYPE_NEG:
            *assoc = -1;
            res = 11;
            break;
        case OPTYPE_POSTDEC:
        case OPTYPE_POSTINC:
        case OPTYPE_PREDEC:
        case OPTYPE_PREINC:
            *assoc = -1;
            res = 12;
            break;
    }
    return res;
}

/*
 * Print operator symbol
 */
static void optype_dump(intend_state *s, op_type type)
{
    switch (type) {
        case OPTYPE_PLUS:
            fprintf(s->stdout, "+");
            break;
        case OPTYPE_MINUS:
            fprintf(s->stdout, "-");
            break;
        case OPTYPE_MUL:
            fprintf(s->stdout, "*");
            break;
        case OPTYPE_DIV:
            fprintf(s->stdout, "/");
            break;
        case OPTYPE_MOD:
            fprintf(s->stdout, "%%");
            break;
        case OPTYPE_BOOL_AND:
            fprintf(s->stdout, "&&");
            break;
        case OPTYPE_BOOL_OR:
            fprintf(s->stdout, "||");
            break;
        case OPTYPE_AND:
            fprintf(s->stdout, "&");
            break;
        case OPTYPE_OR:
            fprintf(s->stdout, "|");
            break;
        case OPTYPE_XOR:
            fprintf(s->stdout, "^");
            break;
        case OPTYPE_POW:
            fprintf(s->stdout, "**");
            break;
        case OPTYPE_EQUAL:
            fprintf(s->stdout, "==");
            break;
        case OPTYPE_NOT_EQUAL:
            fprintf(s->stdout, "!=");
            break;
        case OPTYPE_SEQ:
            fprintf(s->stdout, "<=");
            break;
        case OPTYPE_LEQ:
            fprintf(s->stdout, ">=");
            break;
        case OPTYPE_SMALLER:
            fprintf(s->stdout, "<");
            break;
        case OPTYPE_LARGER:
            fprintf(s->stdout, ">");
            break;
        case OPTYPE_POSTINC:
        case OPTYPE_PREINC:
            fprintf(s->stdout, "++");
            break;
        case OPTYPE_POSTDEC:
        case OPTYPE_PREDEC:
            fprintf(s->stdout, "--");
            break;
        case OPTYPE_NOT:
            fprintf(s->stdout, "!");
            break;
        case OPTYPE_NEG:
            fprintf(s->stdout, "~");
            break;
        case OPTYPE_LSHIFT:
            fprintf(s->stdout, "<<");
            break;
        case OPTYPE_RSHIFT:
            fprintf(s->stdout, ">>");
            break;
    }
}

/*
 * Print function call arguments
 */
static void expr_dump_args(intend_state *s, unsigned int argc, expr **argv)
{
    unsigned int i;

    for (i = 0; i < argc; i++) {
        expr_dump(s, argv[i]);
        if (i + 1 < argc) fprintf(s->stdout, ", ");
    }
}

/*
 * Print array indexing expressions
 */
static void expr_dump_index(intend_state *s, unsigned int argc, expr **argv)
{
    unsigned int i;

    for (i = 0; i < argc; i++) {
        if (argv[i]->type != EXPR_FIELD) {
            fprintf(s->stdout, "[");
            expr_dump(s, argv[i]);
            fprintf(s->stdout, "]");
        } else {
            expr_dump(s, argv[i]);
        }
    }
}

/*
 * Print optional parenthesis around expression
 */
void parenthesis(intend_state *s, expr *outer, expr *inner, int assoc)
{
    int iprec, oprec, iassoc = 0, oassoc = 0;

    oprec = expr_prec(outer, &oassoc);
    iprec = expr_prec(inner, &iassoc);

    if (oprec > iprec || (oprec == iprec && assoc != iassoc)) {
        fprintf(s->stdout, "(");
        expr_dump(s, inner);
        fprintf(s->stdout, ")");
    } else {
        expr_dump(s, inner);
    }
}

/*
 * Indexing without parenthesis allowed?
 */
static int is_indexable(expr_type type)
{
    return (type == EXPR_CALL   ||
            type == EXPR_NEW    ||
            type == EXPR_STATIC ||
            type == EXPR_METHOD ||
            type == EXPR_INDEX);
}

/*
 * Print expression dump
 */
void expr_dump(intend_state *s, expr *ex)
{
    if (!ex) return;

    switch (ex->type) {
        case EXPR_NOP:
            break;
        case EXPR_ASSIGN:
            fprintf(s->stdout, "%s = ", ex->name);
            parenthesis(s, ex, ex->inner, 1);
            break;
        case EXPR_ASSIGN_ARRAY:
            fprintf(s->stdout, "%s", ex->name);
            expr_dump_index(s, ex->argc, ex->argv);
            fprintf(s->stdout, " = ");
            parenthesis(s, ex, ex->inner, 1);
            break;
        case EXPR_CAST:
            fprintf(s->stdout, "(%s) ", ex->name);
            parenthesis(s, ex, ex->inner, 1);
            break;
        case EXPR_CALL:
            fprintf(s->stdout, "%s(", ex->name);
            expr_dump_args(s, ex->argc, ex->argv);
            fprintf(s->stdout, ")");
            break;
        case EXPR_NEW:
            fprintf(s->stdout, "new %s(", ex->name);
            expr_dump_args(s, ex->argc, ex->argv);
            fprintf(s->stdout, ")");
            break;
        case EXPR_STATIC:
            fprintf(s->stdout, "%s::%s(", ex->tname, ex->name);
            expr_dump_args(s, ex->argc, ex->argv);
            fprintf(s->stdout, ")");
            break;
        case EXPR_METHOD:
            expr_dump(s, ex->inner);
            fprintf(s->stdout, ".%s(", ex->name);
            expr_dump_args(s, ex->argc, ex->argv);
            fprintf(s->stdout, ")");
            break;
        case EXPR_REF:
            fprintf(s->stdout, "%s", ex->name);
            break;
        case EXPR_REF_ARRAY:
            fprintf(s->stdout, "%s", ex->name);
            expr_dump_index(s, ex->argc, ex->argv);
            break;
        case EXPR_INDEX:
            if (!is_indexable(ex->inner->type)) {
                fprintf(s->stdout, "(");
            }
            expr_dump(s, ex->inner);
            if (!is_indexable(ex->inner->type)) {
                fprintf(s->stdout, ")");
            }
            expr_dump_index(s, ex->argc, ex->argv);
            break;
        case EXPR_CONST_VOID:
            fprintf(s->stdout, "()");
            break;
        case EXPR_CONST_BOOL:
        case EXPR_CONST_INT:
        case EXPR_CONST_FLOAT:
            fprintf(s->stdout, "%s", ex->name);
            break;
        case EXPR_CONST_STRING:
            fprintf(s->stdout, "\"%s\"", ex->name);
            break;
        case EXPR_INFIX:
            parenthesis(s, ex, ex->inner, -1);
            fprintf(s->stdout, " ");
            optype_dump(s, ex->op);
            fprintf(s->stdout, " ");
            parenthesis(s, ex, ex->index, 1);
            break;
        case EXPR_POSTFIX:
            parenthesis(s, ex, ex->inner, 1);
            optype_dump(s, ex->op);
            break;
        case EXPR_PREFIX:
            optype_dump(s, ex->op);
            parenthesis(s, ex, ex->inner, -1);
            break;
        case EXPR_FIELD:
            fprintf(s->stdout, ".%s", ex->name);
            break;
        case EXPR_STATIC_REF:
            fprintf(s->stdout, "%s::%s", ex->tname, ex->name);
            break;
        case EXPR_LAMBDA:
            stmt_dump_mode(s, 1);
            stmt_dump(s, ex->lambda);
            stmt_dump_mode(s, 0);
            break;
        case EXPR_IF:
            parenthesis(s, ex, ex->inner, 2);
            fprintf(s->stdout, " ? ");
            parenthesis(s, ex, ex->index, 2);
            fprintf(s->stdout, " : ");
            parenthesis(s, ex, ex->elif, 2);
            break;
        case EXPR_PASS_REF:
            fprintf(s->stdout, "&");
            parenthesis(s, ex, ex->inner, 0);
            break;
        case EXPR_FILE:
            fprintf(s->stdout, "__FILE__");
            break;
        case EXPR_LINE:
            fprintf(s->stdout, "__LINE__");
            break;
    }
}
