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

#include "jso_re.h"
#include "jso_string.h"
#include "jso.h"

#include <pcre2.h>

JSO_API jso_re_code *jso_re_code_alloc()
{
	return jso_calloc(1, sizeof(jso_re_code));
}

JSO_API void jso_re_code_free(jso_re_code *code)
{
	if (code != NULL) {
		pcre2_code_free(code->re);
		jso_string_free(code->pattern);
		jso_free(code);
	}
}

JSO_API jso_rc jso_re_compile(jso_string *pattern, jso_re_code *code)
{
	pcre2_code *re = pcre2_compile((PCRE2_SPTR) JSO_STRING_VAL(pattern), PCRE2_ZERO_TERMINATED, 0,
			&code->error_code, &code->error_offset, NULL);
	if (re == NULL) {
		return JSO_FAILURE;
	}
	code->re = re;
	code->pattern = jso_string_copy(pattern);
	return JSO_SUCCESS;
}

JSO_API char *jso_re_get_error_message(jso_re_code *code, jso_ctype *buf, size_t buf_size)
{
	pcre2_get_error_message(code->error_code, buf, buf_size);
	return (char *) buf;
}

JSO_API jso_re_match_data *jso_re_match_data_create(jso_re_code *code)
{
	return pcre2_match_data_create_from_pattern(code->re, NULL);
}

JSO_API void jso_re_match_data_free(jso_re_match_data *match_data)
{
	pcre2_match_data_free(match_data);
}

JSO_API int jso_re_match(
		const char *subject, size_t subject_len, jso_re_code *code, jso_re_match_data *match_data)
{
	return pcre2_match(code->re, (PCRE2_SPTR) subject, subject_len, 0, 0, match_data, NULL);
}
