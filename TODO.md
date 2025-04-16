# TODO list

## Decoding
- Integrate jso schema validation
- Check whether using ZVAL_EMPTY_ARRAY adds some improvements in perf (mainly how it behaves when no array is empy)
  - https://github.com/php/php-src/commit/447f07cd28494caf6c9f08640bc6d355f93ed2f2
- Look into simd parser
- extended error info containing error location
- check if there are some potential issues with bigint checking
  - refactore algorithm in jsond_scanner.re
- use JSON_ERROR_CTRL_CHAR also in non string context (JS condition)

## Encoding
- integrated getting serialization object data for newer versions
  - Support get_properties_for - https://github.com/php/php-src/commit/7ec8087f8097955bfc6b97d1a916c6ffc39908f4
  - Optimized object encoding without rebulding properties HashTable - https://github.com/php/php-src/commit/f9f8c1c79cac1b03279190e0c5513a51881615f9
  - Fix for the above - https://github.com/php/php-src/commit/92aeda524b0b99ab0c861bcba62e5a471ba805e6
  - Fix of the changes above - https://github.com/php/php-src/commit/6057cd2367003146285236f9c731405a8de990a3
- look into better guard protection available in PHP-8.3
  - Introduce Zend guard recursion protection - https://github.com/php/php-src/commit/53aa53f42f9a27aabd66ea632b867b34c6f7a5ae
- look into json serializable minor refactoring
  - Refactor php_json_encode_serializable_object() to call method directly - https://github.com/php/php-src/commit/af1e289332d102e7e4fba00a6809b3da73d83a8b
- look into property hooks support and lazy objects
  - hooks - https://github.com/php/php-src/commit/780a8280d23453ebab5df0dbc35ac897c07c1f0d#diff-7d42a64925fbe4e298b5fcf928cb3994e4e021774503190b850be7d01d181be9
  - lazy object - https://github.com/php/php-src/commit/58aa6fc8308976b9da1cf526bd98d2c7b6b4d2ba#diff-7d42a64925fbe4e298b5fcf928cb3994e4e021774503190b850be7d01d181be9
  - hooks fix - https://github.com/php/php-src/commit/868959350ffd1252b00bfbcdcae8c2d7e229f66c
- Look into generated string (buf) reduction in size
  - Reduce memory allocated by json_encode - https://github.com/php/php-src/commit/4df3dd767912ea129c7874f4b991767d31a44479
- Look into the SIMD changes
- compare utf8 validation with json and optimize it - check the simd one
  - Optimization for ASCII data - https://github.com/php/php-src/commit/46d5551fc345140711bec41c04d93197152d21d0
- Look into conversion to iterative encoding (not recursive)
  - it will allow to get rid of stack checking introduced in https://github.com/php/php-src/commit/a551b99b2ca85dddac2e4428a4df13769c46d6f8 and should improve perf as well
- improve and test native buffer
  - PHPC_CSTRL_RETVAL in PHP_JSON_BUF_RETURN is not ideal as it copies the whole buffer
- add shortcut macro for XSS attacks
  - https://bugs.php.net/bug.php?id=65257

## Build
- add checks for specific re2c and Bison version
- Windows Makefile.frag support - like https://github.com/php/php-src/commit/25ee9465d1c7043f4302329c901fecd38597b634

## Testing
- new generator

# Links

## Testing resources
- Hoa: https://github.com/Hywan/jsond-test
- [Comparison JSON impls](http://gggeek.altervista.org/sw/article_20070425.html) and [its source](https://github.com/gggeek/phpxmlrpc-extras/blob/master/jsonrpc/testsuite.php)
- [Jan Tvrdik compat test](https://gist.github.com/JanTvrdik/10277952#file-test-php)
