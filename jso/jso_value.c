/*
 * Copyright (c) 2012-2025 Jakub Zelenka. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "jso.h"
#include "jso_array.h"
#include "jso_object.h"
#include "jso_pointer.h"
#include "jso_string.h"
#include "jso_schema.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FREEING */

/* free jso value */
JSO_API void jso_value_clear(jso_value *val)
{
	switch (JSO_TYPE_P(val)) {
		case JSO_TYPE_STRING:
			jso_string_free(JSO_STR_P(val));
			break;
		case JSO_TYPE_ARRAY:
			jso_array_free(JSO_ARRVAL_P(val));
			break;
		case JSO_TYPE_OBJECT:
			jso_object_free(JSO_OBJVAL_P(val));
			break;
		case JSO_TYPE_POINTER:
			jso_pointer_free(JSO_PTRVAL_P(val));
			break;
		case JSO_TYPE_SCHEMA_VALUE:
			jso_schema_value_free(JSO_SVVAL_P(val));
			break;
		case JSO_TYPE_ERROR:
			jso_error_free(JSO_EVAL_P(val));
			break;
		default:
			break;
	}
}

JSO_API void jso_value_free(jso_value *val)
{
	jso_value_clear(val);
}

/* COPY */

JSO_API jso_value *jso_value_copy(jso_value *val)
{
	switch (JSO_TYPE_P(val)) {
		case JSO_TYPE_STRING:
			JSO_STR_P(val) = jso_string_copy(JSO_STR_P(val));
			break;
		case JSO_TYPE_ARRAY:
			JSO_ARRVAL_P(val) = jso_array_copy(JSO_ARRVAL_P(val));
			break;
		case JSO_TYPE_OBJECT:
			JSO_OBJVAL_P(val) = jso_object_copy(JSO_OBJVAL_P(val));
			break;
		case JSO_TYPE_POINTER:
			JSO_PTRVAL_P(val) = jso_pointer_copy(JSO_PTRVAL_P(val));
			break;
		case JSO_TYPE_SCHEMA_VALUE:
		case JSO_TYPE_ERROR:
			/* those are not copyable atm. */
			return NULL;
		default:
			break;
	}

	return val;
}

/* ERROR */

/* return error description if val is error */
JSO_API const char *jso_value_get_error_description(jso_value *val)
{
	if (JSO_TYPE_P(val) != JSO_TYPE_ERROR) {
		return NULL;
	}

	return jso_error_type_description(JSO_ETYPE_P(val));
}

/* COMPARISON */
/* check whether two jso values are equal */
JSO_API jso_bool jso_value_equals(jso_value *val1, jso_value *val2)
{
	if (val1->type != val2->type) {
		return false;
	}

	switch (val1->type) {
		case JSO_TYPE_NULL:
			return true;
		case JSO_TYPE_BOOL:
		case JSO_TYPE_INT:
			return JSO_IVAL_P(val1) == JSO_IVAL_P(val2);
		case JSO_TYPE_DOUBLE:
			return JSO_DVAL_P(val1) == JSO_DVAL_P(val2);
		case JSO_TYPE_STRING:
			return jso_string_equals(JSO_STR_P(val1), JSO_STR_P(val2));
		case JSO_TYPE_ARRAY:
			return jso_array_equals(JSO_ARRVAL_P(val1), JSO_ARRVAL_P(val2));
		case JSO_TYPE_OBJECT:
			return jso_object_equals(JSO_OBJVAL_P(val1), JSO_OBJVAL_P(val2));
			break;
		default:
			return false;
	}
}

/* PRINTING */

/* print indentation */
static void jso_value_print_indent(jso_uint indent, jso_io *io)
{
	jso_uint i;
	for (i = 0; i < indent; i++) {
		JSO_IO_PRINTF(io, " ");
	}
}

/* print error */
static void jso_value_print_error(jso_value *val, jso_io *io)
{
	if (!JSO_EVAL_P(val)) {
		JSO_IO_PRINTF(io, "ERROR: Error allocation failed\n");
	} else {
		const char *emsg = jso_error_type_description(JSO_ETYPE_P(val));
		JSO_IO_PRINTF(io, "ERROR: %s: %zu:%zu-%zu:%zu\n", emsg, JSO_ELOC_P(val).first_line,
				JSO_ELOC_P(val).first_column, JSO_ELOC_P(val).last_line,
				JSO_ELOC_P(val).last_column);
	}
}

/**
 * Callback argument
 */
typedef struct _jso_value_dump_callback_arg {
	/** output IO handle */
	jso_io *io;
	/** indention */
	jso_uint indent;
} jso_value_dump_callback_arg;

/* debug print array callback */
static void jso_value_dump_array_callback(size_t idx, jso_value *val, void *varg)
{
	jso_value_dump_callback_arg *arg = (jso_value_dump_callback_arg *) varg;

	jso_value_dump_ex(val, arg->io, arg->indent + 1);
}

/* debug print object callback */
static void jso_value_dump_object_callback(jso_string *key, jso_value *val, void *varg)
{
	jso_value_dump_callback_arg *arg = (jso_value_dump_callback_arg *) varg;

	jso_value_print_indent(arg->indent, arg->io);
	JSO_IO_PRINTF(arg->io, " KEY: %s\n", key->val);
	jso_value_dump_ex(val, arg->io, arg->indent + 1);
}

/* debug print jso value */
JSO_API void jso_value_dump_ex(jso_value *val, jso_io *io, jso_uint indent)
{
	jso_value_print_indent(indent, io);

	switch (JSO_TYPE_P(val)) {
		case JSO_TYPE_NULL:
			JSO_IO_PRINTF(io, "NULL\n");
			break;
		case JSO_TYPE_BOOL:
			JSO_IO_PRINTF(io, "BOOL: %s\n", JSO_IVAL_P(val) ? "TRUE" : "FALSE");
			break;
		case JSO_TYPE_INT:
			JSO_IO_PRINTF(io, "INT: %ld\n", JSO_IVAL_P(val));
			break;
		case JSO_TYPE_DOUBLE:
			JSO_IO_PRINTF(io, "DOUBLE: %f\n", JSO_DVAL_P(val));
			break;
		case JSO_TYPE_STRING:
			if (JSO_SVAL_P(val))
				JSO_IO_PRINTF(
						io, "STRING: \"%s\" ; LENGTH: %zu\n", JSO_SVAL_P(val), JSO_SLEN_P(val));
			else
				JSO_IO_PRINTF(io, "EMPTY STRING\n");
			break;
		case JSO_TYPE_ARRAY:
			if (!JSO_ARRVAL_P(val)) {
				fprintf(stderr, "ERROR: Array allocation failed\n");
			} else {
				jso_value_dump_callback_arg arg = { io, indent };

				JSO_IO_PRINTF(io, "ARRAY:\n");
				jso_array_apply_with_arg(JSO_ARRVAL_P(val), jso_value_dump_array_callback, &arg);
			}
			break;
		case JSO_TYPE_OBJECT:
			if (!JSO_OBJVAL_P(val)) {
				JSO_IO_PRINTF(io, "ERROR: Object allocation failed\n");
			} else {
				jso_value_dump_callback_arg arg = { io, indent };

				JSO_IO_PRINTF(io, "OBJECT:\n");
				jso_object_apply_with_arg(JSO_OBJVAL_P(val), jso_value_dump_object_callback, &arg);
			}
			break;
		case JSO_TYPE_POINTER:
			if (!JSO_PTRVAL_P(val)) {
				JSO_IO_PRINTF(io, "ERROR: Pointer allocation failed\n");
			} else {
				jso_pointer *ptr = JSO_PTRVAL_P(val);
				JSO_IO_PRINTF(io, "POINTER: %s\n", JSO_STRING_VAL(ptr->pointer_value));
			}
			break;
		case JSO_TYPE_ERROR:
			jso_value_print_error(val, io);
			break;
		default:
			break;
	}
}

/* debug print jso value */
JSO_API void jso_value_dump(jso_value *val, jso_io *io)
{
	jso_value_dump_ex(val, io, 0);
}

static const char *type_names[] = { "error", "null", "bool", "int", "double", "string", "array",
	"object", "pointer", "schema" };

JSO_API const char *jso_value_type_to_string(jso_value_type type)
{
	JSO_ASSERT_LT(type, sizeof(type_names) / sizeof(const char *));
	return type_names[(int) type];
}
