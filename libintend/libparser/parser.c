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
 * Intend C Parser Library
 */

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "icl_grammar.h"


/*
 * Initialize the parser data in the state
 */
void parser_alloc(intend_state *s)
{
    s->parser = oom(calloc(sizeof(parser_data), 1));
}

void parser_free(intend_state *s)
{
    cfree(s->parser);
    s->parser = NULL;
}

/*
 * Tear down internal parser memory
 */
void parser_teardown(intend_state *s)
{
    parser_include_teardown(s);
    expr_stack_teardown(s);
    expr_arg_teardown(s);
    stmt_stack_teardown(s);
    stmt_arg_teardown(s);
    parser_free(s);
}

/*
 * Parse file
 *
 * This function tries to parse the given script passed as
 * a file pointer and saves the statement list in the state.
 */
int parser_run(intend_state *s, FILE *file)
{
    parser_data *p;

    sanity(s && file);

    parser_alloc(s);

    p = (parser_data *)s->parser;

    stmt_stack_enter(s);

    // Initialize the lex scanner state and set the lex scanner
    // yyextra var to point back to intend state
    icl_lex_init_extra(s, &(p->scanner));
    // Set the main lex scanner input file
    icl_set_in(file, p->scanner);
    // Do the parsing
    icl_parse(s);
    // Free lex scanner state
    icl_lex_destroy(p->scanner);

    // Did we get an error? Return false
    if (p->parser_error != 0) {
        stmt_stack_leave(s, 1);
        return 0;
    }

    // Save a pointer to the parsed statement stack in state->script
    s->script = stmt_stack_leave(s, 0);

    // Done with success
    return 1;
}
