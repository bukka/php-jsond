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
 * @file jso_io_file.h
 * @brief File IO
 */

#ifndef JSO_IO_FILE_H
#define JSO_IO_FILE_H

#include "jso_io.h"

/**
 * @brief Internal buffer size
 */
#define JSO_IO_FILE_BUFF_SIZE 4096

/**
 * Get `FILE` handle
 * @param io IO handle
 */
#define JSO_IO_FILE_HANDLE_GET(io) ((FILE *) JSO_IO_HANDLE_PTR(io))

/**
 * Set `FILE` handle
 * @param io IO handle
 * @param file_ptr FILE pointer
 */
#define JSO_IO_FILE_HANDLE_SET(io, file_ptr) (JSO_IO_HANDLE_PTR(io) = (void *) (file_ptr))

/**
 * Open file IO from `FILE` handle
 * @param handle `FILE` handle
 * @return New file IO or NULL if error.
 */
JSO_API jso_io *jso_io_file_open_stream(FILE *handle);

/**
 * Open file from path
 * @param filename file path
 * @param opentype open type string as supplied to `fopen`
 * @return New file IO or NULL if error.
 */
JSO_API jso_io *jso_io_file_open(const char *filename, const char *opentype);

/**
 * Close file IO with options to also close std stream
 * @param io IO handle
 * @param close_std wheter to close std streams
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 * @note The standard stream are `stdin`, `stdout` and `stderr`.
 */
JSO_API jso_rc jso_io_file_close_ex(jso_io *io, jso_bool close_std);

/**
 * Close file IO
 * @param io IO handle
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_io_file_close(jso_io *io);

/**
 * Get file size
 * @param filename path of the file
 * @return File size.
 */
JSO_API off_t jso_io_file_size(const char *filename);

#endif /* JSO_IO_FILE_H */
