# Compatibility issue with ext/json

- removed JSON_parser.h header
- removed utf8_decode.h header
- error codes constants moved to php_json.h
  - `enum error_codes` renamed to `php_json_error_codes` (typedef)
  - add JSON_ERROR_UTF16 for invalid \uXXXX code (ill-formatted sorrogate pairs)
  - JSON_ERROR_STATE_MISMATCH is not used
  - macro JSON_PARSER_DEFAULT_DEPTH renamed to PHP_JSON_PARSER_DEFAULT_DEPTH
- ext global `error_code` type changed from `int` to `php_json_error_codes`
