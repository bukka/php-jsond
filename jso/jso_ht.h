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
 * @file jso_ht.h
 * @brief Hash table header
 */

#ifndef JSO_HT_H
#define JSO_HT_H

#include "jso_types.h"

#define JSO_HT_MAX_LOAD 0.75

/**
 * @brief Hash table entry.
 */
typedef struct _jso_ht_entry {
	jso_string *key;
	jso_value value;
	struct _jso_ht_entry *next;
} jso_ht_entry;

/**
 * @brief Hash table.
 */
typedef struct _jso_ht {
	size_t count;
	size_t capacity;
	jso_ht_entry *entries;
	jso_ht_entry *first_entry;
	jso_ht_entry *last_entry;
} jso_ht;

/**
 * @brief Macro to start iteration of the hash table in random order (faster).
 * @param _ht hash table
 * @param _key entry key
 * @param _val entry value pointer
 */
#define JSO_HT_FOREACH_RANDOM(_ht, _key, _val) \
	do { \
		jso_ht_entry *_entries = _ht->entries; \
		jso_ht_entry *_entry; \
		size_t _found = 0; \
		for (size_t _i = 0; _i < _ht->capacity && _found < _ht->count; _i++) { \
			_entry = &_entries[_i]; \
			_key = _entry->key; \
			if (_key == NULL) \
				continue; \
			++_found; \
			_val = &_entry->value;

/**
 * @brief Macro to start iteration of the hash table in stable order.
 * @param _ht hash table
 * @param _key entry key
 * @param _val entry value pointer
 */
#define JSO_HT_FOREACH(_ht, _key, _val) \
	do { \
		for (jso_ht_entry *_entry = _ht->first_entry; _entry; _entry = _entry->next) { \
			_key = _entry->key; \
			_val = &_entry->value;

/**
 * @brief Macro to end iteration of the hash table.
 */
#define JSO_HT_FOREACH_END \
	} \
	} \
	while (0)

/**
 * @brief Macro to start iteration of the hash table in random order (possibly faster).
 * @param _ht hash table
 * @param _key entry key
 * @param _val entry value pointer
 */
#define JSO_HT_FOREACH_RANDOM(_ht, _key, _val) \
	do { \
		jso_ht_entry *_entries = _ht->entries; \
		jso_ht_entry *_entry; \
		size_t _found = 0; \
		for (size_t _i = 0; _i < _ht->capacity && _found < _ht->count; _i++) { \
			_entry = &_entries[_i]; \
			_key = _entry->key; \
			if (_key == NULL) \
				continue; \
			++_found; \
			_val = &_entry->value;

/**
 * Allocate new hash table.
 *
 * @return New initialized hash table.
 */
JSO_API jso_ht *jso_ht_alloc();

/**
 * Initialize hash table.
 *
 * @param ht hash table
 */
JSO_API void jso_ht_init(jso_ht *ht);

/**
 * Free hash table.
 *
 * @param ht hash table
 */
JSO_API void jso_ht_free(jso_ht *ht);

/**
 * Clear hash table (delete all items).
 *
 * @param ht hash table
 */
JSO_API void jso_ht_clear(jso_ht *ht);

/**
 * Resize hash table.
 *
 * @param ht hash table
 * @param size new size
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 * @note Only increasing of size is allowed.
 */
JSO_API jso_rc jso_ht_resize(jso_ht *ht, size_t size);

/**
 * Set value to the hash table.
 *
 * @param ht hash table
 * @param key key of the value
 * @param value value to set
 * @param free_old whether to free old value if update is executed
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_ht_set(jso_ht *ht, jso_string *key, jso_value *value, jso_bool free_old);

/**
 * Get value from the hash table.
 *
 * @param ht hash table
 * @param key key of the value to find
 * @param value pointer that gets the returned value pointer
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_ht_get(jso_ht *ht, jso_string *key, jso_value **value);

/**
 * Check whether key exist in the hash table.
 *
 * @param obj object
 * @param key key of the value
 * @return @ref JSO_TRUE if key exists, otherwise @ref JSO_FALSE.
 */
JSO_API jso_bool jso_ht_has(jso_ht *ht, jso_string *key);

/**
 * Get value from the hash table by C string key.
 *
 * @param ht hash table
 * @param key C string key of the value to find
 * @param key_len C string key length
 * @param value pointer that gets the returned value pointer
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_ht_get_by_cstr_key_with_len(
		jso_ht *ht, const char *key, size_t key_len, jso_value **value);

/**
 * Get value from the hash table by C string key.
 *
 * @param ht hash table
 * @param key C string key of the value to find
 * @param value pointer that gets the returned value pointer
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_ht_get_by_cstr_key(jso_ht *ht, const char *key, jso_value **value);

/**
 * Clone entries from one table to another.
 *
 * @param from source hash table
 * @param to destination hash table
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_ht_clone(jso_ht *from, jso_ht *to);

#endif /* JSO_HT_H */
