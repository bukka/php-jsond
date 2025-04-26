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

/**
 * @file jso_value.h
 * @brief JSO value
 */

#ifndef JSO_VALUE_H
#define JSO_VALUE_H

#include "jso_types.h"
#include "jso_string.h"
#include "jso_io.h"

/**
 * Get a type of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Type @ref jso_value_type.
 */
#define JSO_TYPE_P(_pjv) (_pjv)->type

/**
 * Get an integer value of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Integer value.
 */
#define JSO_IVAL_P(_pjv) (_pjv)->data.ival

/**
 * Get a double value of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Double precision value.
 */
#define JSO_DVAL_P(_pjv) (_pjv)->data.dval

/**
 * Get a string of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_string.
 */
#define JSO_STR_P(_pjv) (_pjv)->data.str

/**
 * Get a string value of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Pointer to the string value (characters buffer).
 */
#define JSO_SVAL_P(_pjv) JSO_STRING_VAL(JSO_STR_P(_pjv))

/**
 * Get a string length of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return String length.
 */
#define JSO_SLEN_P(_pjv) JSO_STRING_LEN(JSO_STR_P(_pjv))

/**
 * Get a cstr string value of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Pointer to the cstr value (const char buffer).
 */
#define JSO_CSVAL_P(_pjv) (const char *) JSO_SVAL_P(_pjv)

/**
 * Get an array of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_array.
 */
#define JSO_ARRVAL_P(_pjv) (_pjv)->data.arr

/**
 * Get an object of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_object.
 */
#define JSO_OBJVAL_P(_pjv) (_pjv)->data.obj

/**
 * Get an object hash table of the supplied pointer to value.
 *
 * @param _pjv variable of @ref jso_value type
 * @return Pointer to @ref jso_ht.
 */
#define JSO_OBJHVAL_P(_pjv) &(_pjv)->data.obj->ht

/**
 * Get a JsonPointer of the supplied pointer to value.
 *
 * @param _pjv variable of @ref jso_value type
 * @return Pointer to @ref jso_pointer.
 */
#define JSO_PTRVAL_P(_pjv) (_pjv)->data.ptr

/**
 * Get a schema value of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_schema_value.
 */
#define JSO_SVVAL_P(_pjv) (_pjv)->data.sobj

/**
 * Get an error of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_error or `NULL` if no error.
 */
#define JSO_EVAL_P(_pjv) (_pjv)->data.err

/**
 * Get an error location of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_error_location.
 * @note This must be called only if the error is set.
 */
#define JSO_ELOC_P(_pjv) (_pjv)->data.err->loc

/**
 * Get an error type of the supplied pointer to value.
 *
 * @param _pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_error_type.
 * @note This must be called only if the error is set.
 */
#define JSO_ETYPE_P(_pjv) (_pjv)->data.err->type

/**
 * Get a type of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Type @ref jso_value_type.
 */
#define JSO_TYPE(_jv) JSO_TYPE_P(&(_jv))

/**
 * Get an integer value of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Integer value.
 */
#define JSO_IVAL(_jv) JSO_IVAL_P(&(_jv))

/**
 * Get a double value of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Double precision value.
 */
#define JSO_DVAL(_jv) JSO_DVAL_P(&(_jv))

/**
 * Get a string of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Pointer to @ref jso_string.
 */
#define JSO_STR(_jv) JSO_STR_P(&(_jv))

/**
 * Get a string value of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return String value (characters buffer).
 */
#define JSO_SVAL(_jv) JSO_SVAL_P(&(_jv))

/**
 * Get a string length of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return String length.
 */
#define JSO_SLEN(_jv) JSO_SLEN_P(&(_jv))

/**
 * Get a C string value of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return C string value (const char buffer).
 */
#define JSO_CSVAL(_jv) JSO_CSVAL_P(&(_jv))

/**
 * Get an array of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Pointer to @ref jso_array.
 */
#define JSO_ARRVAL(_jv) JSO_ARRVAL_P(&(_jv))

/**
 * Get an object of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Pointer to @ref jso_object.
 */
#define JSO_OBJVAL(_jv) JSO_OBJVAL_P(&(_jv))

/**
 * Get an object hash table of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Pointer to @ref jso_ht.
 */
#define JSO_OBJHVAL(_jv) JSO_OBJHVAL_P(&(_jv))

/**
 * Get a JsonPointer of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Pointer to @ref jso_pointer.
 */
#define JSO_PTRVAL(_jv) JSO_PTRVAL_P(&(_jv))

/**
 * Get a schema value of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Pointer to @ref jso_schema_value.
 */
#define JSO_SVVAL(_jv) JSO_SVVAL_P(&(_jv))

/**
 * Get an error of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Pointer to @ref jso_error or `NULL` if no error.
 */
#define JSO_EVAL(_jv) JSO_EVAL_P(&(_jv))

/**
 * Get an error location of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Pointer to @ref jso_error_location .
 * @note This must be called only if the error is set.
 */
#define JSO_ELOC(_jv) JSO_ELOC_P(&(_jv))

/**
 * Get an error type of the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @return Pointer to @ref jso_error_type.
 * @note This must be called only if the error is set.
 */
#define JSO_ETYPE(_jv) JSO_ETYPE_P(&(_jv))

/**
 * Set error to the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @param _eval error value
 */
#define JSO_VALUE_SET_ERROR(_jv, _eval) \
	do { \
		JSO_TYPE(_jv) = JSO_TYPE_ERROR; \
		JSO_EVAL(_jv) = _eval; \
	} while (0)

/**
 * Set null to the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 */
#define JSO_VALUE_SET_NULL(_jv) \
	do { \
		JSO_TYPE(_jv) = JSO_TYPE_NULL; \
	} while (0)

/**
 * Set boolean to the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @param _bv boolean value
 */
#define JSO_VALUE_SET_BOOL(_jv, _bv) \
	do { \
		JSO_TYPE(_jv) = JSO_TYPE_BOOL; \
		JSO_IVAL(_jv) = _bv; \
	} while (0)

/**
 * Set an integer value to the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @param _lv integer value
 */
#define JSO_VALUE_SET_INT(_jv, _lv) \
	do { \
		JSO_TYPE(_jv) = JSO_TYPE_INT; \
		JSO_IVAL(_jv) = _lv; \
	} while (0)

/**
 * Set a double floating point number to the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @param _dv double floating point value
 */
#define JSO_VALUE_SET_DOUBLE(_jv, _dv) \
	do { \
		JSO_TYPE(_jv) = JSO_TYPE_DOUBLE; \
		JSO_DVAL(_jv) = (_dv); \
	} while (0)

/**
 * Set a string to the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @param _sv string value (character buffer)
 * @param sl string length
 */
#define JSO_VALUE_SET_STRING(_jv, _sv) \
	do { \
		JSO_TYPE(_jv) = JSO_TYPE_STRING; \
		JSO_STR(_jv) = _sv; \
	} while (0)

/**
 * Set an array to the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @param _av array
 */
#define JSO_VALUE_SET_ARRAY(_jv, _av) \
	do { \
		JSO_TYPE(_jv) = JSO_TYPE_ARRAY; \
		JSO_ARRVAL(_jv) = (_av); \
	} while (0)

/**
 * Set an object to the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @param _ov object
 */
#define JSO_VALUE_SET_OBJECT(_jv, _ov) \
	do { \
		JSO_TYPE(_jv) = JSO_TYPE_OBJECT; \
		JSO_OBJVAL(_jv) = (_ov); \
	} while (0)

/**
 * Set a pointer to the supplied value.
 *
 * @param _jv variable of @ref jso_value type
 * @param _pv pointer
 */
#define JSO_VALUE_SET_POINTER(_jv, _pv) \
	do { \
		JSO_TYPE(_jv) = JSO_TYPE_POINTER; \
		JSO_PTRVAL(_jv) = (_pv); \
	} while (0)

/**
 * Set a schema value to the supplied value.
 *
 * @param _pjv variable of @ref jso_value type
 * @param _sv schema value
 */
#define JSO_VALUE_SET_SCHEMA_VALUE(_pjv, _sv) \
	do { \
		JSO_TYPE(_pjv) = JSO_TYPE_SCHEMA_VALUE; \
		JSO_SVVAL(_pjv) = (_sv); \
	} while (0)

/**
 * Set null to the supplied value pointer.
 *
 * @param _pjv pointer to variable of @ref jso_value type
 */
#define JSO_VALUE_SET_NULL_P(_pjv) \
	do { \
		JSO_TYPE_P(_pjv) = JSO_TYPE_NULL; \
	} while (0)

/**
 * Set boolean to the supplied value pointer.
 *
 * @param _pjv pointer to variable of @ref jso_value type
 * @param _bv boolean value
 */
#define JSO_VALUE_SET_BOOL_P(_pjv, _bv) \
	do { \
		JSO_TYPE_P(_pjv) = JSO_TYPE_BOOL; \
		JSO_IVAL_P(_pjv) = _bv; \
	} while (0)

/**
 * Set an integer value to the supplied value pointer.
 *
 * @param _pjv pointer to variable of @ref jso_value type
 * @param _lv integer value
 */
#define JSO_VALUE_SET_INT_P(_pjv, _lv) \
	do { \
		JSO_TYPE_P(_pjv) = JSO_TYPE_INT; \
		JSO_IVAL_P(_pjv) = _lv; \
	} while (0)

/**
 * Set a double floating point number to the supplied value pointer.
 *
 * @param _pjv pointer to variable of @ref jso_value type
 * @param _dv double floating point value
 */
#define JSO_VALUE_SET_DOUBLE_P(_pjv, _dv) \
	do { \
		JSO_TYPE_P(_pjv) = JSO_TYPE_DOUBLE; \
		JSO_DVAL_P(_pjv) = (_dv); \
	} while (0)

/**
 * Set a string to the supplied value pointer.
 *
 * @param _pjv pointer to variable of @ref jso_value type
 * @param _sv string value (character buffer)
 * @param sl string length
 */
#define JSO_VALUE_SET_STRING_P(_pjv, _sv) \
	do { \
		JSO_TYPE_P(_pjv) = JSO_TYPE_STRING; \
		JSO_STR_P(_pjv) = _sv; \
	} while (0)

/**
 * Set an array to the supplied value pointer.
 *
 * @param _pjv pointer to variable of @ref jso_value type
 * @param _av array
 */
#define JSO_VALUE_SET_ARRAY_P(_pjv, _av) \
	do { \
		JSO_TYPE_P(_pjv) = JSO_TYPE_ARRAY; \
		JSO_ARRVAL_P(_pjv) = (_av); \
	} while (0)

/**
 * Set an object to the supplied value pointer.
 *
 * @param _pjv pointer to variable of @ref jso_value type
 * @param _ov object
 */
#define JSO_VALUE_SET_OBJECT_P(_pjv, _ov) \
	do { \
		JSO_TYPE_P(_pjv) = JSO_TYPE_OBJECT; \
		JSO_OBJVAL_P(_pjv) = (_ov); \
	} while (0)

/**
 * Set a pointer (JsonPointer) to the supplied value pointer.
 *
 * @param _pjv pointer to variable of @ref jso_value type
 * @param _pv pointer
 */
#define JSO_VALUE_SET_POINTER_P(_pjv, _pv) \
	do { \
		JSO_TYPE_P(_pjv) = JSO_TYPE_POINTER; \
		JSO_PTRVAL_P(_pjv) = (_pv); \
	} while (0)

/**
 * Set a schema value to the supplied value pointer.
 *
 * @param _pjv pointer to variable of @ref jso_value type
 * @param _sv schema value
 */
#define JSO_VALUE_SET_SCHEMA_VALUE_P(_pjv, _sv) \
	do { \
		JSO_TYPE_P(_pjv) = JSO_TYPE_SCHEMA_VALUE; \
		JSO_SVVAL_P(_pjv) = (_sv); \
	} while (0)

/**
 * Clear @ref jso_value instance.
 *
 * @param val instance of @ref jso_value
 */
JSO_API void jso_value_clear(jso_value *val);

/**
 * Clear @ref jso_value instance.
 *
 * @param val instance of @ref jso_value
 * @deprecated Use jso_value_clear
 */
JSO_API void jso_value_free(jso_value *val);

/**
 * Copy @ref jso_value instance.
 *
 * @param val instance of @ref jso_value
 * @return Copied @ref jso_value instance
 */
JSO_API jso_value *jso_value_copy(jso_value *val);

/**
 * Return error type for the provided value.
 *
 * @param val value
 * @return Error description or NULL if val is not an error
 */
JSO_API const char *jso_value_get_error_description(jso_value *val);

/**
 * Check if two values are equal.
 *
 * @param val1 first value
 * @param val2 second value
 * @return Error description or NULL if val is not an error
 */
JSO_API jso_bool jso_value_equals(jso_value *val1, jso_value *val2);

/**
 * Print indented debug output of the supplied value.
 *
 * @param val instance of @ref jso_value
 * @param io output IO handler
 * @param indent number of spaces for initial indention
 */
JSO_API void jso_value_dump_ex(jso_value *val, jso_io *io, jso_uint indent);

/**
 * Print debug output of the supplied value.
 *
 * @param val instance of @ref jso_value
 * @param io output IO handler
 */
JSO_API void jso_value_dump(jso_value *val, jso_io *io);

/**
 * Return string representation of the @ref jso_value_type.
 *
 * @param type value type.
 * @return The string representation of the type.
 */
JSO_API const char *jso_value_type_to_string(jso_value_type type);

#endif /* JSO_VALUE_H */
