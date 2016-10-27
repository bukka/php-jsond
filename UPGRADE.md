# Upgrading from the previous versions

## 1.4

### User visible changes

#### Decoder
- Empty string is considered invalid and JSON_ERROR_SYNTAX is raised
- Removed option `JSOND_VALID_ESCAPED_UNICODE`
  - The `JSON_ERROR_UTF16` will be always set for invalid \uXXXX code
- Added new error `JSOND_ERROR_INVALID_PROPERTY_NAME` for invalid property names
  - The only case when the property is invalid is if it starts with `\0` character

## 1.3

### User visible changes

#### Decoder
- Rejected ECMA-404 incompatible number formats
  - top level (PHP json_decode check): `07`, `0xff`, `.1`, `-.1`
  - all (JSON_Parser): `[1.]`, [1.e1]
- Add new option `JSOND_VALID_ESCAPED_UNICODE` to check if \uXXXX code is not ill-formed surrogate pair
  - If set, the new error `JSON_ERROR_UTF16` will be set for invalid \uXXXX code

#### Encoder
- Fixed Bug 64695
  - JSON_NUMERIC_CHECK has issues with strings that are numbers plus the letter

### Internal API changes
- removed JSON_parser.h header
- removed utf8_decode.h header
- error codes constants moved to php_json.h
- `enum error_codes` renamed to `php_json_error_codes` (typedef)
-  ext global `error_code` type changed from `int` to `php_json_error_codes`
- macro JSON_PARSER_DEFAULT_DEPTH renamed to PHP_JSON_PARSER_DEFAULT_DEPTH


## 1.2

Version 1.2 of jsond has never been released as it is a version of the json extension in PHP 5.
