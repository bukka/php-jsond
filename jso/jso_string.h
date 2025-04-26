/*
 * Copyright (c) 2023 Jakub Zelenka. All rights reserved.
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
 * @file jso_string.h
 * @brief String header
 */

#ifndef JSO_STRING_H
#define JSO_STRING_H

#include "jso_types.h"
#include "jso_mm.h"

#include <string.h>

/**
 * @brief String flag defining whether the hash was already set.
 */
#define JSO_STRING_FLAG_HASH_SET 1

/**
 * @brief Character type for scanner.
 */
typedef unsigned char jso_ctype;

/**
 * @brief String type.
 */
struct _jso_string {
	/* reference count */
	jso_uint16 refcount;
	/* string flags */
	jso_uint16 flags;
	/* hash value */
	jso_uint32 hash;
	/** string length */
	size_t len;
	/** string characters */
	jso_ctype val[1];
};

/**
 * Get string length for the supplied string pointer.
 *
 * @param _str pointer to @ref jso_string
 * @return The string length
 */
#define JSO_STRING_LEN(_str) (_str)->len

/**
 * Get string value for the supplied string pointer.
 *
 * @param _str pointer to @ref jso_string
 * @return The string value (jso_ctype array)
 */
#define JSO_STRING_VAL(_str) ((jso_ctype *) &(_str)->val)

/**
 * Get a C string value for the supplied string pointer.
 *
 * @param _str pointer to @ref jso_string
 * @return The string value (const char array)
 */
#define JSO_STRING_CSTR_VAL(_str) ((const char *) &(_str)->val)

/**
 * Get string flags of the supplied string pointer.
 *
 * @param _str pointer to @ref jso_string
 * @return String flags.
 */
#define JSO_STRING_FLAGS(_str) (_str)->flags

/**
 * Get hash value count of the supplied string pointer.
 *
 * @param _str pointer to @ref jso_string
 * @return Hash value.
 */
#define JSO_STRING_HASH(_str) (_str)->hash

/**
 * Get reference count of the supplied string pointer.
 *
 * @param _str pointer to @ref jso_string
 * @return References count value.
 */
#define JSO_STRING_REFCOUNT(_str) (_str)->refcount

/**
 * Allocate string.
 *
 * @param len string length
 * @return Newly allocated string.
 */
static inline jso_string *jso_string_alloc(size_t len)
{
	return jso_calloc(1, sizeof(jso_string) + len);
}

/**
 * Allocate string and copy C string into it.
 *
 * @param cstr C string to copy
 * @param len C string length
 * @return Newly allocated string containing C string.
 */
static inline jso_string *jso_string_create_from_cstr_len(const char *cstr, size_t len)
{
	jso_string *str = jso_string_alloc(len);
	if (str == NULL) {
		return NULL;
	}
	JSO_STRING_LEN(str) = len;
	memcpy(JSO_STRING_VAL(str), cstr, len);
	JSO_STRING_VAL(str)[len] = '\0';

	return str;
}

/**
 * Allocate string and copy C string into it.
 *
 * @param cstr C string to copy
 * @return Newly allocated string containing C string.
 */
static inline jso_string *jso_string_create_from_cstr(const char *cstr)
{
	return jso_string_create_from_cstr_len(cstr, strlen(cstr));
}

/**
 * Free string.
 *
 * @param str string to free
 */
static inline void jso_string_free(jso_string *str)
{
	if (str == NULL) {
		return;
	}
	if (JSO_STRING_REFCOUNT(str) > 0) {
		--JSO_STRING_REFCOUNT(str);
	} else {
		jso_free(str);
	}
}

/**
 * Copy string.
 *
 * @param str string to copy
 * @return The copied string.
 */
static inline jso_string *jso_string_copy(jso_string *str)
{
	++JSO_STRING_REFCOUNT(str);
	return str;
}

/**
 * Check wheter two strings are equal.
 *
 * @param s1 first string
 * @param s2 second string
 * @return True if strings are equal, otherwise false.
 */
static inline bool jso_string_equals(jso_string *s1, jso_string *s2)
{
	return s1 == s2
			|| (JSO_STRING_LEN(s1) == JSO_STRING_LEN(s2)
					&& !memcmp(JSO_STRING_VAL(s1), JSO_STRING_VAL(s2), JSO_STRING_LEN(s1)));
}

/**
 * Check wheter two strings are equal.
 *
 * @param s1 first string
 * @param s2 second string
 * @return True if strings are equal, otherwise false.
 */
static inline bool jso_string_prefix_equals(jso_string *s1, jso_string *s2, size_t prefix_len)
{
	return s1 == s2
			|| (JSO_STRING_LEN(s1) >= prefix_len && JSO_STRING_LEN(s2) >= prefix_len
					&& !memcmp(JSO_STRING_VAL(s1), JSO_STRING_VAL(s2), prefix_len));
}

/**
 * Check wheter a string is equal to C string.
 *
 * @param str string to compare
 * @param cstr C string to compare
 * @return True if strings are equal, otherwise false.
 */
static inline bool jso_string_equals_to_cstr(jso_string *str, const char *cstr)
{
	size_t str_len = JSO_STRING_LEN(str);
	return strlen(cstr) == str_len && !memcmp(JSO_STRING_VAL(str), cstr, str_len);
}

/**
 * Convert string to integer.
 *
 * @param str string to convert
 * @param result Converted number of type @ref jso_int
 * @return JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
static inline jso_rc jso_string_to_int(jso_string *str, jso_int *result)
{
	char *endptr;
	long val = strtol((char *) JSO_STRING_VAL(str), &endptr, 10);
	if (endptr != (char *) (JSO_STRING_VAL(str) + JSO_STRING_LEN(str))) {
		return JSO_FAILURE;
	}
	*result = val;
	return JSO_SUCCESS;
}

/**
 * Set string hash.
 *
 * @param str string
 */
static inline void jso_string_set_hash(jso_string *str, jso_uint32 hash)
{
	JSO_STRING_HASH(str) = hash;
	JSO_STRING_FLAGS(str) |= JSO_STRING_FLAG_HASH_SET;
}

/**
 * Check whether string hash is set.
 *
 * @param str string
 * @return True if hash is set, otherwise false.
 */
static inline bool jso_string_has_hash(jso_string *str)
{
	return JSO_STRING_FLAGS(str) & JSO_STRING_FLAG_HASH_SET;
}

/**
 * Return string hash.
 *
 * @param str string
 * @return String hash.
 */
static inline jso_uint32 jso_string_get_hash(jso_string *str)
{
	return JSO_STRING_HASH(str);
}

/**
 * @brief Value representing not found string position
 */
#define JSO_STRING_POS_NOT_FOUND -1

/**
 * Return position of the character first occurrence in the string.
 *
 * @param str string
 * @param ch character to find
 * @param offset offset to start the search from
 * @return Position of the character in the string or @ref JSO_STRING_POS_NOT_FOUND.
 */
static inline ssize_t jso_string_find_char_pos(jso_string *str, char ch, size_t offset)
{
	jso_ctype *pch = memchr(JSO_STRING_VAL(str) + offset, (int) ch, str->len);
	return pch == NULL ? JSO_STRING_POS_NOT_FOUND : pch - JSO_STRING_VAL(str);
}

/**
 * Concat prefixes of the strings.
 *
 * @param s1 the fists string
 * @param s1_len the fists string length of the prefix
 * @param s2 the second string
 * @param s2_len the second string length of the prefix
 * @return New string containing concatenated string prefixes or NULL on error.
 */
static inline jso_string *jso_string_concat_prefixes(
		jso_string *s1, size_t s1_len, jso_string *s2, size_t s2_len)
{
	size_t len = s1_len + s2_len;
	jso_string *str = jso_string_alloc(len);
	if (str == NULL) {
		return str;
	}
	memcpy(JSO_STRING_VAL(str), JSO_STRING_VAL(s1), s1_len);
	memcpy(&JSO_STRING_VAL(str)[s1_len], JSO_STRING_VAL(s2), s2_len);
	JSO_STRING_VAL(str)[len] = '\0';
	JSO_STRING_LEN(str) = len;
	return str;
}

/**
 * Concat two strings.
 *
 * @param s1 the fists string
 * @param s2 the second string
 * @return New string containing concatenated string prefixes or NULL on error.
 */
static inline jso_string *jso_string_concat(jso_string *s1, jso_string *s2)
{
	return jso_string_concat_prefixes(s1, JSO_STRING_LEN(s1), s2, JSO_STRING_LEN(s2));
}

/**
 * Create substring from the string.
 *
 * @param str the main string
 * @param offset offset from the start of the main string to copy
 * @param len lenght to copy from the main string
 * @return New substring.
 */
static inline jso_string *jso_string_substring(jso_string *str, size_t offset, size_t len)
{
	if (offset == 0 && len == JSO_STRING_LEN(str)) {
		return jso_string_copy(str);
	}
	if (len + offset > JSO_STRING_LEN(str)) {
		return NULL;
	}
	jso_string *substr = jso_string_alloc(len);
	if (substr == NULL) {
		return NULL;
	}
	JSO_STRING_LEN(substr) = len;
	memcpy(JSO_STRING_VAL(substr), JSO_STRING_VAL(str) + offset, len);
	JSO_STRING_VAL(substr)[len] = '\0';

	return substr;
}

#endif /* JSO_STRING_H */
