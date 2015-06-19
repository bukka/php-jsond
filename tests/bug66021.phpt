--TEST--
Bug #66021 (Blank line inside empty array/object when JSON_PRETTY_PRINT is set)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

class Foo {
	private $bar = 'baz';
}

echo $jsond_encode([[], (object)[], new Foo], jsond_constant('PRETTY_PRINT')), "\n";

?>
--EXPECT--
[
    [],
    {},
    {}
]