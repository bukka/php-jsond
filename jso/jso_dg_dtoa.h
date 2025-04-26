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
 * @file jso_dg_dtoa.h
 * @brief Header for dtoa from David M. Gray
 */

#ifndef JSO_DG_DTOA_H
#define JSO_DG_DTOA_H

#ifdef Long
#undef Long
#endif

/**
 * Long is defined as int because dtoa works only with 32bit integers
 */
#define Long int

#if defined(IEEE_8087) + defined(IEEE_MC68k) + defined(VAX) + defined(IBM) != 1
/**
 * IEEE 8087 is used on most current platform
 */
#define IEEE_8087
#endif

/**
 * Convert string to double.
 * @param s00 string to convert
 * @param se pointer to the last converted byte
 *        of the supplied string `s00` will be saved there
 * @return Resulted double.
 * @note This is an internal function that is not exported.
 */
double jso_dg_strtod(const char *s00, char **se);

/**
 * Double to string conversion function.
 * @param d double precision number to convert
 * @param mode conversion mode
 * @param ndigits number of digits to use (not applied for mode 0)
 * @param decpt resulted number of decimal place
 * @param sign number sign
 * @param rve pointer to string end will be saved there
 * @return Resulted string.
 * @note This is an internal function that is not exported.
 */
char *jso_dg_dtoa(double d, int mode, int ndigits, int *decpt, int *sign, char **rve);

/**
 * Free dtoa allocated string.
 * @param s string to frees
 */
void jso_dg_freedtoa(char *s);

#endif /* JSO_DG_DTOA_H */
