# TODO list

## Decoding
- use JSON_ERROR_CTRL_CHAR also in non string context (JS condition)
- replace ill-formed characters with substitute character
  - https://bugs.php.net/bug.php?id=65082
  - https://bugs.php.net/bug.php?id=63898
- extended error info containing error location
- check if there are some potential issues with bigint checking
  - refactore algorithm in jsond_scanner.re

## Encoding
- use serialize_precision for encoding of double values
- backport json encode fail return
  - https://github.com/php/php-src/pull/2101
  - https://bugs.php.net/bug.php?id=68992
  - https://bugs.php.net/bug.php?id=70275
- backport json_encoder context - fixes stacking of exceptions in JsonSerializable objects
  - https://github.com/php/php-src/pull/2173
  - https://bugs.php.net/bug.php?id=66025
  - https://bugs.php.net/bug.php?id=73254
- verify (add test) and possibly fix segfault with throwing JsonSerializable
  - https://bugs.php.net/bug.php?id=73113
- add shortcut macro for XSS attacks
  - https://bugs.php.net/bug.php?id=65257
- improve and test native buffer
  - PHPC_CSTRL_RETVAL in PHP_JSON_BUF_RETURN is not ideal as it copies the whole buffer

## Build
- add checks for specific re2c and Bison version

## Testing
- new generator

# Links

## Testing resources
- Hoa: https://github.com/Hywan/jsond-test
- [Comparison JSON impls](http://gggeek.altervista.org/sw/article_20070425.html) and [its source](https://github.com/gggeek/phpxmlrpc-extras/blob/master/jsonrpc/testsuite.php)
- [Jan Tvrdik compat test](https://gist.github.com/JanTvrdik/10277952#file-test-php)
