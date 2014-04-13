# TODO list

## Decoding
- error for `{ "\u0000 key starting with zero": "value" }` (`JSON_ERROR_MANGLED_PROPERTY_NAME`)
- error for empty input (it will be incompatible with json ext but it's compatible with standard)
- extended error info containing error location

## Encoding
- use new UTF8 validation (https://github.com/php/php-src/pull/636)

## General
- Replace functions and constants if PHP_JSOND_PRIMARY defined

## Build
- add configuration option for define PHP_JSOND_PRIMARY
- use modified version of PHP_PROG_BISON that only sets $YACC but won't fail the build
- add new check for RE2C

## Testing
- fork json-test-suite and modify it to generate customizable test sets
- possible ideas
  - Hoa: https://github.com/Hywan/jsond-test
  - [Comparison JSON impls](http://gggeek.altervista.org/sw/article_20070425.html) and [its source](https://github.com/gggeek/phpxmlrpc-extras/blob/master/jsonrpc/testsuite.php)
  - [Jan Tvrdik compat test](https://gist.github.com/JanTvrdik/10277952#file-test-php)
