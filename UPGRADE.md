# Compatibility issues with ext/json

## User visible changes

### Decoder
- Rejected ECMA-404 incompatible number formats
  - top level (PHP json_decode check): `07`, `0xff`, `.1`, `-.1`
  - all (JSON_Parser): `[1.]`, [1.e1]
- New error `JSON_ERROR_UTF16` for invalid \uXXXX code (ill-formed surrogate pairs)

### Encoder
- Added new option `JSON_PRESERVE_FRACTIONAL_PART`
  - feature request 50224 ( keeping the float type when encoding )
  - modified version of https://github.com/php/php-src/pull/642

## Internal API changes
- removed JSON_parser.h header
- removed utf8_decode.h header
- error codes constants moved to php_json.h
- `enum error_codes` renamed to `php_json_error_codes` (typedef)
-  ext global `error_code` type changed from `int` to `php_json_error_codes`
- macro JSON_PARSER_DEFAULT_DEPTH renamed to PHP_JSON_PARSER_DEFAULT_DEPTH

