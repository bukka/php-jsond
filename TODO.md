# TODO list

## Decoding
- error for `{ "\u0000 key starting with zero": "value" }` (`JSON_ERROR_MANGLED_PROPERTY_NAME`)
- extended error info containing error location
- fix potential issues with bigint checking
  - check for max double string length (use string if it's too big)
  - refactore algorithm in jsond_scanner.re

## Encoding
- fix, improve and test native buffer

## General
- Replace functions and constants if PHP_JSOND_PRIMARY defined

## Build
- add configuration option for define PHP_JSOND_PRIMARY
- add checks for specific re2c and Bison version

## Testing
- new generator
- more sources
  - Hoa: https://github.com/Hywan/jsond-test
  - [Comparison JSON impls](http://gggeek.altervista.org/sw/article_20070425.html) and [its source](https://github.com/gggeek/phpxmlrpc-extras/blob/master/jsonrpc/testsuite.php)
  - [Jan Tvrdik compat test](https://gist.github.com/JanTvrdik/10277952#file-test-php)
