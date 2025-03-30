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
- Look into generated string (buf) reduction in size
  - Reduce memory allocated by json_encode - https://github.com/php/php-src/commit/4df3dd767912ea129c7874f4b991767d31a44479
- Look into the SIMD changes
- compare utf8 validation with json and optimize it - check the simd one
  - Optimization for ASCII data - https://github.com/php/php-src/commit/46d5551fc345140711bec41c04d93197152d21d0
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
