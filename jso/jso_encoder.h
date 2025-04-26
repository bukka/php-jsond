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
 * @file jso_encoder.h
 * @brief Encoder
 */

#ifndef JSO_ENCODER_H
#define JSO_ENCODER_H

#include "jso_types.h"
#include "jso_io.h"

/**
 * Constant value for unlimited depth.
 */
#define JSO_ENCODER_DEPTH_UNLIMITED -1

/**
 * Indent string for pretty printing.
 */
#define JSO_ENCODER_PRETTY_INDENT "    "

/**
 * Elements separator string for pretty printing.
 */
#define JSO_ENCODER_PRETTY_ELSEP "\n"

/**
 * Key-value separator for pretty printing.
 */
#define JSO_ENCODER_PRETTY_KVSEP " "

/**
 * @brief Options for encoder.
 */
typedef struct _jso_encoder_options {
	/** maximal depth that can be encoded */
	jso_int max_depth;
	/** whether to use pretty printing */
	jso_bool pretty;
} jso_encoder_options;

/**
 * @brief Encoder instance handle structure.
 */
typedef struct _jso_encoder {
	/** encoder output IO */
	jso_io *output;
	/** encoder options */
	jso_encoder_options options;
	/* actual depth */
	jso_int depth;
} jso_encoder;

/**
 * Initialize encoder.
 *
 * @param encoder encoder instance
 * @param output output IO
 * @param options options
 */
JSO_API void jso_encoder_init(jso_encoder *encoder, jso_io *output, jso_encoder_options *options);

/**
 * Encode value using the encoder.
 *
 * @param encoder encoder instance
 * @param val value to encode
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_encoder_encode(jso_encoder *encoder, jso_value *val);

/**
 * Encode supplied value.
 *
 * @param val value to encode
 * @param output output IO
 * @param options options
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_encode(jso_value *val, jso_io *output, jso_encoder_options *options);

#endif /* JSO_ENCODER_H */
