/*
 * Copyright (c) 2025 Jakub Zelenka. All rights reserved.
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
 * @file jso_dbg.h
 * @brief Debugging utilities
 */

#ifndef JSO_DBG_H
#define JSO_DBG_H

#include <stdarg.h>
#include "../config.h"

#define JSO_DBG_DEFAULT_ENV_NAME "JSO_DEBUG_CONFIG"

#ifdef JSO_DEBUG_ENABLED

void jso_dbg_init_from_config(const char *config_string);
void jso_dbg_init_from_env(const char *env_name);
void jso_dbg_log(const char *type, const char *fmt, ...);
void jso_dbg_cleanup(void);

#define JSO_DBG(_type, ...) jso_dbg_log(#_type, __VA_ARGS__)

#else
#define jso_dbg_init_from_config(_config) ((void) 0)
#define jso_dbg_init_from_env(_config) ((void) 0)
#define jso_dbg_cleanup() ((void) 0)
#define JSO_DBG(...) ((void) 0)
#endif

/**
 * @brief Schema validation component debugging
 */
#define JSO_DBG_SV(...) JSO_DBG(SV, __VA_ARGS__)

#endif /* JSO_DBG_H */
