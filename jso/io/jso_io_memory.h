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
 * @file jso_io_memory.h
 * @brief Memory IO
 */

#ifndef JSO_IO_MEMORY_H
#define JSO_IO_MEMORY_H

#include "jso_io.h"

/**
 * @brief Internal buffer size
 */
#define JSO_IO_MEMORY_BUFF_SIZE 128

/**
 * Open memory IO with supplied initial size and flags
 * @param size allocated size when created
 * @param flags extra customization flags
 * @return New IO.
 * @note There are no flags defined as yet.
 */
JSO_API jso_io *jso_io_momory_open_ex(size_t size, int flags);

/**
 * Open memory IO with default initial size and flags
 * @return New IO.
 */
JSO_API jso_io *jso_io_momory_open();

#endif // JSO_IO_MEMORY_H
