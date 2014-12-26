--TEST--
Bug #66021 (Blank line inside empty array/object when JSOND_PRETTY_PRINT is set)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php

class Foo {
	private $bar = 'baz';
}

echo jsond_encode([[], (object)[], new Foo], JSOND_PRETTY_PRINT), "\n";

?>
--EXPECT--
[
    [],
    {},
    {}
]