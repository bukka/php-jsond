/*
 * Copyright (c) 2012-2023 Jakub Zelenka. All rights reserved.
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
 * @file jso_scanner.h
 * @brief Lexical scanner.
 */

#ifndef JSO_SCANNER_H
#define JSO_SCANNER_H

#include "jso.h"
#include "jso_io.h"

/**
 * @brief Scanner data and state.
 *
 * This is the main state and data structure used in scanner.
 */
typedef struct _jso_scanner {
	jso_io *io;
	jso_value value;
	jso_ctype *pstr;
	int state;
	jso_error_location loc;
} jso_scanner;

/**
 * @brief Current scanning location.
 */
#define JSO_SCANNER_LOCATION(scanner, slocation) (scanner).loc.slocation

/**
 * @brief Initialize scanner.
 *
 * @param scanner Scanner instance.
 * @param io IO structure.
 */
void jso_scanner_init(jso_scanner *scanner, jso_io *io);

/**
 * @brief Run scanner.
 *
 * @param s Scanner instance.
 * @return int Token ID.
 */
int jso_scan(jso_scanner *s);

#endif /* JSO_SCANNER_H */
