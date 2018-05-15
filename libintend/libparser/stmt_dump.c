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
 * Intend C Statement dumping
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

/*
 * Print indentation prefix
 */
static void prefix(intend_state *s)
{
    int i;

    if (s->dump_mode == 0) {
        for (i = 0; i < s->dump_depth * 2; i++) {
            putchar(' ');
        }
    }
}

static void newline(intend_state *s)
{
    if (s->dump_mode == 0) {
        fprintf(s->stdout, "\n");
    } else {
        fprintf(s->stdout, " ");
    }
}

/*
 * Print function arguments dump
 */
static void stmt_proto_dump(intend_state *s, char *args, char **names)
{
    if (!args) return;

    while (*args != 0) {
        if (isupper(*args)) {
            fprintf(s->stdout, "forced ");
        }
        switch (tolower(*args)) {
            case 'v':
                fprintf(s->stdout, "void");
                break;
            case 'b':
                fprintf(s->stdout, "bool");
                break;
            case 'i':
                fprintf(s->stdout, "int");
                break;
            case 'f':
                fprintf(s->stdout, "float");
                break;
            case 's':
                fprintf(s->stdout, "string");
                break;
            case 'a':
                fprintf(s->stdout, "array");
                break;
            case 'p':
                fprintf(s->stdout, "fn");
                break;
            case '?':
                fprintf(s->stdout, "mixed");
                break;
        }
        if (names) {
            fprintf(s->stdout, " %s", *names);
        }
        ++args;
        if (*args != 0) {
            fprintf(s->stdout, ", ");
        }
    }
}

/*
 * Dump function
 */
static void stmt_dump_func(intend_state *s, stmt *st)
{
    char buf[2];

    if (s->dump_mode == 0) {
        buf[0] = st->rettype;
        buf[1] = 0;
        stmt_proto_dump(s, buf, NULL);
        fprintf(s->stdout, " %s(", st->name);
    } else {
        fprintf(s->stdout, "\\ (");
    }
    stmt_proto_dump(s, st->proto, st->names);
    fprintf(s->stdout, ")"); newline(s);
    stmt_dump(s, st->true_case);
}

/*
 * Print statement dump
 */
void stmt_dump(intend_state *s, stmt *st)
{
    sanity(st);

    prefix(s);
    switch (st->type) {
        case STMT_NOP:
            fprintf(s->stdout, ";"); newline(s);
            break;
        case STMT_BLOCK:
            fprintf(s->stdout, "{"); newline(s);
            ++s->dump_depth;
            stmt_list_dump(s, (stmt_list *) st->block, 1);
            --s->dump_depth;
            prefix(s);
            fprintf(s->stdout, "}"); newline(s);
            break;
        case STMT_IF:
            fprintf(s->stdout, "if (");
            expr_dump(s, st->expr);
            fprintf(s->stdout, ")"); newline(s);
            ++s->dump_depth;
            stmt_dump(s, st->true_case);
            --s->dump_depth;
            break;
        case STMT_IF_ELSE:
            fprintf(s->stdout, "if (");
            expr_dump(s, st->expr);
            fprintf(s->stdout, ")"); newline(s);
            ++s->dump_depth;
            stmt_dump(s, st->true_case);
            --s->dump_depth;
            prefix(s);
            fprintf(s->stdout, "else"); newline(s);
            ++s->dump_depth;
            stmt_dump(s, st->false_case);
            --s->dump_depth;
            break;
        case STMT_WHILE:
            fprintf(s->stdout, "while (");
            expr_dump(s, st->expr);
            fprintf(s->stdout, ")"); newline(s);
            ++s->dump_depth;
            stmt_dump(s, st->true_case);
            --s->dump_depth;
            break;
        case STMT_DO:
            fprintf(s->stdout, "do"); newline(s);
            ++s->dump_depth;
            stmt_dump(s, st->true_case);
            --s->dump_depth;
            prefix(s);
            fprintf(s->stdout, "while (");
            expr_dump(s, st->expr);
            fprintf(s->stdout, ")");
            break;
        case STMT_FOR:
            fprintf(s->stdout, "for (");
            expr_dump(s, st->init);
            fprintf(s->stdout, "; ");
            expr_dump(s, st->expr);
            fprintf(s->stdout, "; ");
            expr_dump(s, st->guard);
            fprintf(s->stdout, ")"); newline(s);
            ++s->dump_depth;
            stmt_dump(s, st->true_case);
            --s->dump_depth;
            break;
        case STMT_CONTINUE:
            fprintf(s->stdout, "continue;"); newline(s);
            break;
        case STMT_BREAK:
            fprintf(s->stdout, "break;"); newline(s);
            break;
        case STMT_RETURN:
            fprintf(s->stdout, "return");
            if (st->expr) {
                fprintf(s->stdout, " ");
                expr_dump(s, st->expr);
            }
            fprintf(s->stdout, ";"); newline(s);
            break;
        case STMT_EXPR:
            expr_dump(s, st->expr);
            fprintf(s->stdout, ";"); newline(s);
            break;
        case STMT_FUNC:
            stmt_dump_func(s, st);
            break;
        case STMT_SWITCH:
            fprintf(s->stdout, "switch (");
            expr_dump(s, st->expr);
            fprintf(s->stdout, ") {"); newline(s);
            ++s->dump_depth;
            stmt_list_dump(s, (stmt_list *) st->block, 1);
            --s->dump_depth;
            prefix(s);
            fprintf(s->stdout, "}"); newline(s);
            break;
        case STMT_CASE:
            fprintf(s->stdout, "case ");
            expr_dump(s, st->expr);
            fprintf(s->stdout, ":"); newline(s);
            ++s->dump_depth;
            stmt_list_dump(s, (stmt_list *) st->block, 1);
            if (!st->thru) {
                prefix(s);
                fprintf(s->stdout, "break;"); newline(s);
            }
            --s->dump_depth;
            break;
        case STMT_DEFAULT:
            fprintf(s->stdout, "default:"); newline(s);
            ++s->dump_depth;
            stmt_list_dump(s, (stmt_list *) st->block, 1);
            --s->dump_depth;
            break;
        case STMT_TRY:
            fprintf(s->stdout, "try"); newline(s);
            ++s->dump_depth;
            stmt_dump(s, st->true_case);
            --s->dump_depth;
            prefix(s);
            fprintf(s->stdout, "catch (%s)", st->name); newline(s);
            ++s->dump_depth;
            stmt_dump(s, st->false_case);
            --s->dump_depth;
            break;
        case STMT_THROW:
            fprintf(s->stdout, "throw ");
            expr_dump(s, st->expr);
            fprintf(s->stdout, ";"); newline(s);
            break;
        case STMT_CLASS:
            fprintf(s->stdout, "class %s", st->name);
            if (st->proto) {
                fprintf(s->stdout, " extends %s", st->proto);
            }
            newline(s);
            prefix(s);
            fprintf(s->stdout, "{"); newline(s);
            ++s->dump_depth;
            stmt_list_dump(s, (stmt_list *) st->block, 1);
            --s->dump_depth;
            prefix(s);
            fprintf(s->stdout, "}"); newline(s);
    }
}

/*
 * Print statement list dump
 */
void stmt_list_dump(intend_state *s, stmt_list *list, int internal)
{
    unsigned int i;

    if (!internal) s->dump_depth = 0;
    if (!list) return;

    for (i = 0; i < list->len; i++) {
        stmt_dump(s, list->list[i]);
    }
}

/*
 * Set dump mode
 */
void stmt_dump_mode(intend_state *s, int mode)
{
    if (mode) {
        ++s->dump_mode;
    } else {
        --s->dump_mode;
    }
}
