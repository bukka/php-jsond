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

#include "jso.h"
#include "jso_ht.h"

#include <stdlib.h>
#include <string.h>

/* Use FNV-1a hash function. */
static jso_uint32 jso_ht_create_hash(const jso_ctype *val, size_t len)
{
	jso_uint32 hash = 2166136261u;
	for (size_t i = 0; i < len; i++) {
		hash ^= (jso_uint32) val[i];
		hash *= 16777619;
	}
	return hash;
}

static inline jso_uint32 jso_ht_create_string_hash(jso_string *str)
{
	return jso_ht_create_hash(JSO_STRING_VAL(str), JSO_STRING_LEN(str));
}

static inline jso_uint32 jso_ht_get_string_hash(jso_string *str)
{
	return jso_string_has_hash(str) ? jso_string_get_hash(str) : jso_ht_create_string_hash(str);
}

static inline jso_ht_entry *jso_ht_find_entry_by_cstr_key(
		jso_ht_entry *entries, size_t capacity, const char *ckey, size_t key_len)
{
	jso_uint32 index = jso_ht_create_hash((const jso_ctype *) ckey, key_len) % capacity;
	while (1) {
		jso_ht_entry *entry = &entries[index];
		if (entry->key == NULL || jso_string_equals_to_cstr(entry->key, ckey)) {
			return entry;
		}

		index = (index + 1) % capacity;
	}
}

static inline jso_ht_entry *jso_ht_find_entry(
		jso_ht_entry *entries, size_t capacity, jso_string *key)
{
	jso_uint32 index = jso_ht_get_string_hash(key) % capacity;
	while (1) {
		jso_ht_entry *entry = &entries[index];
		if (entry->key == NULL || jso_string_equals(entry->key, key)) {
			return entry;
		}

		index = (index + 1) % capacity;
	}
}

static jso_rc jso_ht_adjust_capacity(jso_ht *ht, size_t capacity)
{
	jso_ht_entry *entries = jso_calloc(capacity, sizeof(jso_ht_entry));
	if (entries == NULL) {
		return JSO_FAILURE;
	}

	jso_ht_entry *first_entry = NULL, *last_entry = NULL;
	for (jso_ht_entry *src_entry = ht->first_entry; src_entry; src_entry = src_entry->next) {
		jso_ht_entry *dest_entry = jso_ht_find_entry(entries, capacity, src_entry->key);
		dest_entry->key = src_entry->key;
		dest_entry->value = src_entry->value;
		if (last_entry) {
			last_entry->next = dest_entry;
		} else {
			first_entry = dest_entry;
		}
		last_entry = dest_entry;
	}

	jso_free(ht->entries);
	ht->entries = entries;
	ht->capacity = capacity;
	ht->first_entry = first_entry;
	ht->last_entry = last_entry;

	return JSO_SUCCESS;
}

static void jso_ht_free_entries(jso_ht *ht)
{
	jso_string *key;
	jso_value *val;

	JSO_HT_FOREACH_RANDOM(ht, key, val)
	{
		if (JSO_STRING_VAL(key)) {
			jso_string_free(key);
		}
		jso_value_free(val);
	}
	JSO_HT_FOREACH_END;

	jso_free(ht->entries);
}

JSO_API jso_ht *jso_ht_alloc()
{
	return jso_calloc(1, sizeof(jso_ht));
}

JSO_API void jso_ht_init(jso_ht *ht)
{
	memset(ht, 0, sizeof(jso_ht));
}

JSO_API void jso_ht_free(jso_ht *ht)
{
	jso_ht_free_entries(ht);
	jso_free(ht);
}

JSO_API void jso_ht_clear(jso_ht *ht)
{
	jso_ht_free_entries(ht);
	jso_ht_init(ht);
}

JSO_API jso_rc jso_ht_resize(jso_ht *ht, size_t size)
{
	size_t capacity = (size_t) (size * (1 / JSO_HT_MAX_LOAD) + 1);
	if (capacity <= ht->capacity) {
		return capacity == ht->capacity ? JSO_SUCCESS : JSO_FAILURE;
	}
	return jso_ht_adjust_capacity(ht, capacity);
}

JSO_API jso_rc jso_ht_set(jso_ht *ht, jso_string *key, jso_value *value, jso_bool free_old)
{
	if (ht->count + 1 > ht->capacity * JSO_HT_MAX_LOAD) {
		size_t capacity = (ht->capacity + 1) * 2;
		if (jso_ht_adjust_capacity(ht, capacity) == JSO_FAILURE) {
			return JSO_FAILURE;
		}
	}

	jso_ht_entry *entry = jso_ht_find_entry(ht->entries, ht->capacity, key);
	jso_bool is_new_key = entry->key == NULL;
	if (is_new_key) {
		++ht->count;
		if (ht->last_entry) {
			ht->last_entry->next = entry;
			ht->last_entry = entry;
		} else {
			ht->first_entry = ht->last_entry = entry;
		}
	} else if (free_old) {
		jso_string_free(entry->key);
	}

	entry->key = key;
	entry->value = *value;

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_ht_get(jso_ht *ht, jso_string *key, jso_value **value)
{
	if (ht->count == 0) {
		return JSO_FAILURE;
	}

	jso_ht_entry *entry = jso_ht_find_entry(ht->entries, ht->capacity, key);
	if (entry->key == NULL) {
		return JSO_FAILURE;
	}

	*value = &entry->value;
	return JSO_SUCCESS;
}

JSO_API jso_bool jso_ht_has(jso_ht *ht, jso_string *key)
{
	if (ht->count == 0) {
		return false;
	}

	jso_ht_entry *entry = jso_ht_find_entry(ht->entries, ht->capacity, key);
	if (entry->key == NULL) {
		return false;
	}

	return true;
}

JSO_API jso_rc jso_ht_get_by_cstr_key_with_len(
		jso_ht *ht, const char *key, size_t key_len, jso_value **value)
{
	if (ht->count == 0) {
		return JSO_FAILURE;
	}

	jso_ht_entry *entry = jso_ht_find_entry_by_cstr_key(ht->entries, ht->capacity, key, key_len);
	if (entry->key == NULL) {
		return JSO_FAILURE;
	}

	*value = &entry->value;
	return JSO_SUCCESS;
}

JSO_API jso_rc jso_ht_get_by_cstr_key(jso_ht *ht, const char *key, jso_value **value)
{
	return jso_ht_get_by_cstr_key_with_len(ht, key, strlen(key), value);
}

JSO_API jso_rc jso_ht_clone(jso_ht *from, jso_ht *to)
{
	for (jso_ht_entry *entry = from->first_entry; entry; entry = entry->next) {

		if (jso_ht_set(to, jso_string_copy(entry->key), &entry->value, true) == JSO_FAILURE) {
			return JSO_FAILURE;
		}
	}
	return JSO_SUCCESS;
}
