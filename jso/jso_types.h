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
 * @file jso_types.h
 * @brief Type definitions
 */

#ifndef JSO_TYPES_H
#define JSO_TYPES_H

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Exported library functions.
 */
#define JSO_API

/**
 * @brief Boolean type.
 */
typedef bool jso_bool;

/**
 * @brief Integer type.
 */
typedef long jso_int;

/**
 * @brief Unsigned integer type.
 */
typedef unsigned long jso_uint;

/**
 * @brief Unsigned integer 16bit type.
 */
typedef uint16_t jso_uint16;

/**
 * @brief Unsigned integer 32bit type.
 */
typedef uint32_t jso_uint32;

/**
 * @brief Unsigned integer 64bit type.
 */
typedef uint64_t jso_uint64;

/**
 * @brief Double type.
 */
typedef double jso_double;

/**
 * @brief Double or int typetype.
 */
typedef struct _jso_number {
	jso_bool is_int;
	union {
		jso_double dval;
		jso_int ival;
	};
} jso_number;

/**
 * @brief Character type for scanner.
 */
typedef unsigned char jso_ctype;

/**
 * @brief String type.
 */
typedef struct _jso_string jso_string;

/**
 * @brief Array type.
 */
typedef struct _jso_array jso_array;

/**
 * @brief Object type.
 */
typedef struct _jso_object jso_object;

/**
 * @brief Regular expression code.
 */
typedef struct _jso_re_code jso_re_code;

/**
 * @brief Pointer type.
 */
typedef struct _jso_pointer jso_pointer;

/**
 * @brief Schema value type.
 */
typedef struct _jso_schema_value jso_schema_value;

/**
 * @brief Value type.
 */
typedef enum {
	/** error value type */
	JSO_TYPE_ERROR = 0,
	/** null value type */
	JSO_TYPE_NULL = 1,
	/** boolean value type */
	JSO_TYPE_BOOL = 2,
	/** integer value type */
	JSO_TYPE_INT = 3,
	/** double value type */
	JSO_TYPE_DOUBLE = 4,
	/** string value type */
	JSO_TYPE_STRING = 5,
	/** array value type */
	JSO_TYPE_ARRAY = 6,
	/** object value type */
	JSO_TYPE_OBJECT = 7,
	/** pointer type */
	JSO_TYPE_POINTER = 8,
	/** schema object value type */
	JSO_TYPE_SCHEMA_VALUE = 9,
} jso_value_type;

/**
 * @brief Error type.
 */
typedef enum {
	/** no error */
	JSO_ERROR_NONE,
	/** allocation error */
	JSO_ERROR_ALLOC,
	/** syntax error */
	JSO_ERROR_SYNTAX,
	/** maximal depth exceeded error */
	JSO_ERROR_DEPTH,
	/** invalid token error */
	JSO_ERROR_TOKEN,
	/** control character error */
	JSO_ERROR_CTRL_CHAR,
	/** invalid unicode escape error */
	JSO_ERROR_UTF16,
	/** UTF-8 encoding error */
	JSO_ERROR_UTF8,
	/** invalid escape error */
	JSO_ERROR_ESCAPE
} jso_error_type;

/**
 * @brief Error location.
 *
 * It identifies a place where the error was found. The structure is designed for Bison parser.
 */
typedef struct _jso_error_location {
	/** first column of the error */
	size_t first_column;
	/** first line of the error */
	size_t first_line;
	/** last column of the error */
	size_t last_column;
	/** last line of the error */
	size_t last_line;
} jso_error_location;

/**
 * @brief Error information structure.
 */
typedef struct _jso_error {
	/** error type */
	jso_error_type type;
	/** error location */
	jso_error_location loc;
} jso_error;

/**
 * @brief Value data.
 *
 * The current size of the value data is equal to the largest element which is 64bit for double.
 */
typedef union _jso_value_data {
	/** integer value */
	jso_int ival;
	/** double value */
	jso_double dval;
	/** string value (reference) */
	jso_string *str;
	/** array value (reference) */
	jso_array *arr;
	/** object value (reference) */
	jso_object *obj;
	/** pointer (reference) */
	jso_pointer *ptr;
	/** json schema object (reference) */
	jso_schema_value *sobj;
	/** error value (reference) */
	jso_error *err;
} jso_value_data;

/**
 * @brief Value data and type.
 *
 * The size of the structure 128 bits on 64bit platforms and 96 bits on 32bit platforms.
 */
typedef struct _jso_value {
	/** value data */
	jso_value_data data;
	/** value type */
	jso_value_type type;
} jso_value;

/**
 * @brief Return code.
 */
typedef enum _jso_rc {
	/** success return code */
	JSO_SUCCESS = 0,
	/** failure return code */
	JSO_FAILURE = -1
} jso_rc;

/**
 * @brief TRUE value.
 */
#define JSO_TRUE true

/**
 * @brief Boolean FALSE value.
 */
#define JSO_FALSE false

#endif /* JSO_TYPES_H */
