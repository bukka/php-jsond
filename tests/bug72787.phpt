--TEST--
Bug #72787 (json_decode reads out of bounds)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_decode('[]', false, 0x100000000));

?>
--EXPECTF--

Warning: %s(): Depth must be lower than %d in %s on line %d
NULL
