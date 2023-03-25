# Upgrading from the previous versions

## 2.0

### User visible changes

### General
- Added compatibility with PHP 8.0
- Add `JSOND_INVALID_UTF8_IGNORE` and `JSOND_INVALID_UTF8_SUBSTITUTE` options for `jsond_encode`
  and `jsond_decode` to ignore or replace invalid UTF-8 byte sequences - it addresses a feature
  request #65082.

#### Decoder
- Added parser method API including `jsond_parser_method` for hooking JSON parser
- Allowed `JSOND_OBJECT_AS_ARRAY` option to actually have meaning

#### Encoder
- Backported `jsond_encoder` context - fixes stacking of exceptions in `JsondSerializable` objects
  - https://github.com/php/php-src/pull/2173
  - https://bugs.php.net/bug.php?id=66025
  - https://bugs.php.net/bug.php?id=73254
- Fixed segfault with throwing JsondSerializable
  - https://bugs.php.net/bug.php?id=73113
- Escaped U+2028 and U+2029 when `JSOND_UNESCAPED_UNICODE` is supplied as `jsond_encode`
  options and added `JSOND_UNESCAPED_LINE_TERMINATORS` to restore the previous behaviour
- Fixed behavior of `JsondSerializable` difference from `jsond_encode` when error
  - PHP bug #72069
- Used serialize_precision instead of precision for encoding of double values

## 1.4

### User visible changes

#### Decoder
- Empty string is considered invalid and `JSOND_ERROR_SYNTAX` is raised
- Removed option `JSOND_VALID_ESCAPED_UNICODE`
  - The `JSOND_ERROR_UTF16` will be always set for invalid \uXXXX code
  - PHP bug #62010
- Added new error `JSOND_ERROR_INVALID_PROPERTY_NAME` for invalid property names
  - The only case when the property is invalid is if it starts with `\0` character
  - PHP bug #68546
- Fixed compatibility with json ext for  `JSOND_ERROR_CTRL_CHAR`

#### Encoder
- Added depth checking for greater than 0 and lower than `INT_MAX`
  - PHP bug #72787
- Added `JSOND_PRESERVE_ZERO_FRACTION` option for better handling of float values
- Fixed blank line inside empty array/object when JSOND_PRETTY_PRINT is set
  - PHP bug #66021
- Fixed `JSOND_NUMERIC_CHECK` issue with NaN and Inf double
  - PHP bug #64695

### Internal changes
- Added compatibility with PHP 7
- Used new UTF8 decoder based on automaton from Bjoern Hoehrmann
- Introduced new experimental buffer for encoding
  - Added a build option to enable it: `--enable-jsond-buffer-native`
- Added a possibility to switch prefix to json and replace existing json functions and constants
  - Added a build option to use json prefixing: `--enable-jsond-with-json-prefix`
- Use special template for Bison
  - Version 3.0.4 has to be used
- The parser and scanner is rebuilt only if enabled in configuration
  - Added a build option to rebuild it: `--enable-jsond-filegen`


## 1.3

### User visible changes

#### Decoder
- Rejected ECMA-404 incompatible number formats
  - top level (PHP json_decode check): `07`, `0xff`, `.1`, `-.1`
  - all (JSON_Parser): `[1.]`, [1.e1]
- Added new option `JSOND_VALID_ESCAPED_UNICODE` to check if \uXXXX code is not ill-formed surrogate pair
  - If set, the new error `JSOND_ERROR_UTF16` will be set for invalid \uXXXX code

### Internal changes
- Removed JSON_parser.h header
- Removed utf8_decode.h header
- Error codes constants moved to php_json.h
- Renamed `enum error_codes` to `php_json_error_codes` (typedef)
- Changed ext global `error_code` type from `int` to `php_json_error_codes` enum
- Renamed macro JSON_PARSER_DEFAULT_DEPTH to PHP_JSON_PARSER_DEFAULT_DEPTH


## 1.2

Version 1.2 of jsond has never been released as it is a version of the json extension in PHP 5.
