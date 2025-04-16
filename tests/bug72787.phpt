--TEST--
Bug #72787 (jsond_decode reads out of bounds)
--EXTENSIONS--
jsond
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
require_once "bootstrap.inc";

try {
    var_dump(jsond_decode('[]', false, 0x100000000));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
jsond_decode(): Argument #3 ($depth) must be less than %d
