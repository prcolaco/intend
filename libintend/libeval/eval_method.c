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
 * Intend C Method call evaluation
 */

#include <stdio.h>
#include <stdlib.h>

#include "eval.h"

/*
 * Enter struct namespace
 */
static void enter_struct(intend_state *s, value *val)
{
    symtab_stack_enter(s);
    symtab_stack_add_variable(s, "this", val);
}

/*
 * Leave struct namespace
 */
static value *leave_struct(intend_state *s, signature *sig,
                           unsigned int argc, expr **argv)
{
    symtab_entry *entry;
    value *res = NULL;

    entry = symtab_stack_lookup(s, "this");
    if (entry) {
        res = value_copy(&entry->entry_u.var);
    }
    update_call_args(s, sig, argc, argv);
    return res;
}

/*
 * Recursively construct instance
 */
static void getinstance(intend_state *s, const char *name)
{
    symtab_entry *entry;

    entry = symtab_stack_lookup(s, name);
    if (!entry || entry->type != SYMTAB_ENTRY_CLASS) {
        fatal(s, "use of undefined class `%s'", name);
        return;
    }
    if (entry->entry_u.cls.parent) {
        getinstance(s, entry->entry_u.cls.parent);
    }
    if (entry->entry_u.cls.type == CLASS_TYPE_USERDEF) {
        eval_stmt_list(s, (stmt_list *) entry->entry_u.cls.definition, 0);
    } else {
        // The class is builtin, so register it
        symtab_stack_add_function(s, entry->symbol, entry->entry_u.cls.constructor);
    }

    entry = symtab_stack_lookup(s, name);
    if (entry && entry->type == SYMTAB_ENTRY_FUNCTION) {
        s->new_cons = entry->symbol;
        s->new_sig  = &(entry->entry_u.fnc.sigs[0]);
    }
}

/*
 * Evaluate constructor expression
 */
value *eval_new(intend_state *s, expr *ex)
{
    value *res, *tname;

    sanity(ex && ex->name);

    symtab_stack_enter(s);
    s->new_cons = NULL;
    s->new_sig  = NULL;
    getinstance(s, ex->name);

    if (s->except_flag || s->exit_flag) {
        symtab_stack_leave(s);
        return value_make_void();
    }

    res = value_make_struct();
    symtab_free(res->value_u.struct_val);
    res->value_u.struct_val = symtab_stack_pop(s);

    tname = value_make_string(ex->name);
    value_set_struct(res, "__class", tname);
    value_free(tname);

    if (s->new_cons) {
        value **argv;
        value *temp, *cons;

        eval_call_args(s, ex->argc, ex->argv, &argv);
        enter_struct(s, res);

        cons = call_function(s, s->new_sig, ex->argc, argv);
        value_free(cons);

        temp = leave_struct(s, s->new_sig, ex->argc, ex->argv);
        if (!temp) {
            fatal(s, "no `this' at constructor `%s' exit", s->new_cons);
            temp = value_make_void();
        }
        free_call_args(s, ex->argc, &argv);

        value_free(res);
        res = temp;
    }

    return res;
}

/*
 * Evaluate static method call
 */
value *eval_static(intend_state *s, expr *ex)
{
    symtab_entry *entry;
    signature *sig;
    value **argv, *res;

    sanity(ex && ex->tname && ex->name);

    symtab_stack_enter(s);
    getinstance(s, ex->tname);

    if (s->except_flag || s->exit_flag) {
        symtab_stack_leave(s);
        return value_make_void();
    }

    entry = symtab_stack_lookup(s, ex->name);
    if (!entry || entry->type != SYMTAB_ENTRY_FUNCTION ||
            !symtab_stack_local(s, ex->name)) {
        fatal(s, "call to undefined method `%s::%s'", ex->tname, ex->name);
        symtab_stack_leave(s);
        return value_make_void();
    }
    sig = &(entry->entry_u.fnc.sigs[0]);
    symtab_stack_leave(s);

    eval_call_args(s, ex->argc, ex->argv, &argv);
    symtab_stack_enter(s);

    res = call_function(s, sig, ex->argc, argv);
    update_call_args(s, sig, ex->argc, ex->argv);

    free_call_args(s, ex->argc, &argv);

    return res;
}

/*
 * Evaluate static reference
 */
value *eval_static_ref(intend_state *s, expr *ex)
{
    symtab *sym;
    symtab_entry *entry;
    value *res;

    sanity(ex && ex->tname && ex->name);

    symtab_stack_enter(s);
    getinstance(s, ex->tname);
    sym = symtab_stack_pop(s);

    entry = symtab_lookup(sym, ex->name);
    if (!entry) {
        fatal(s, "use of undefined class member `%s::%s'", ex->tname,
              ex->name);
        symtab_free(sym);
        return value_make_void();
    }
    if (entry->type == SYMTAB_ENTRY_VAR) {
        res = value_copy(&entry->entry_u.var);
    } else {
        res = value_make_fn(&(entry->entry_u.fnc.sigs[0]));
    }
    symtab_free(sym);
    return res;
}

/*
 * Evaluate method call
 */
value *eval_method(intend_state *s, expr *ex)
{
    symtab_entry *entry;
    value *val, *res, *temp, **argv;

    sanity(ex && ex->inner && ex->name);

    val = eval_expr(s, ex->inner);
    if (val->type != VALUE_TYPE_STRUCT) {
        value_free(val);
        fatal(s, "method call on non-struct value");
        return value_make_void();
    }

    entry = symtab_lookup(val->value_u.struct_val, ex->name);
    if (!entry || entry->type != SYMTAB_ENTRY_FUNCTION) {
        value_free(val);
        fatal(s, "call to undefined method `%s'", ex->name);
        return value_make_void();
    }

    eval_call_args(s, ex->argc, ex->argv, &argv);
    enter_struct(s, val);

    res = call_function(s, &(entry->entry_u.fnc.sigs[0]), ex->argc, argv);

    temp = leave_struct(s, &(entry->entry_u.fnc.sigs[0]), ex->argc, ex->argv);
    free_call_args(s, ex->argc, &argv);

    value_free(val);

    if (temp) {
        if (ex->inner->type == EXPR_REF) {
            symtab_stack_add_variable(s, ex->inner->name, temp);
        }
        if (ex->inner->type == EXPR_REF_ARRAY) {
            eval_assign_array_direct(s, ex->inner->name, ex->inner->argc,
                                     ex->inner->argv, temp);
        }
        value_free(temp);
    }

    return res;
}
