--TEST--
Bug #66021 (Blank line inside empty array/object when JSON_PRETTY_PRINT is set)
--EXTENSIONS--
jsond
--FILE--
<?php
require_once "bootstrap.inc";

class Foo {
    private $bar = 'baz';
}

echo jsond_encode(array(array(), (object) array(), new Foo), JSOND_PRETTY_PRINT), "\n";

?>
--EXPECT--
[
    [],
    {},
    {}
]
