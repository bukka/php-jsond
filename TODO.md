# TODO list

## Decoding
- use JSON_ERROR_CTRL_CHAR also in non string context (JS condition)
- error for `{ "\u0000 key starting with zero": "value" }` (`JSON_ERROR_MANGLED_PROPERTY_NAME`)
  - https://bugs.php.net/bug.php?id=68546
- add options for checking UTF8 validity for surrogate pairs escapes
  - https://bugs.php.net/bug.php?id=62010
- extended error info containing error location
- check if there are some potential issues with bigint checking
  - refactore algorithm in jsond_scanner.re

## Encoding
- fix stacking of exceptions in JsonSerializable objects
  - https://bugs.php.net/bug.php?id=68992
- add shortcut macro for XSS attacks
  - https://bugs.php.net/bug.php?id=65257
- improve and test native buffer

## General
- Replace functions and constants if PHP_JSOND_PRIMARY defined

## Build
- add configuration option for define PHP_JSOND_PRIMARY
- add checks for specific re2c and Bison version

## Testing
- new generator

# Links

## Testing resources
- Hoa: https://github.com/Hywan/jsond-test
- [Comparison JSON impls](http://gggeek.altervista.org/sw/article_20070425.html) and [its source](https://github.com/gggeek/phpxmlrpc-extras/blob/master/jsonrpc/testsuite.php)
- [Jan Tvrdik compat test](https://gist.github.com/JanTvrdik/10277952#file-test-php)
