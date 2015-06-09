--TEST--
Bug #45791 (json_decode() does not handle number 0e0)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_decode('{"zero": 0e0}'));

?>
--EXPECT--
object(stdClass)#1 (1) {
  ["zero"]=>
  float(0)
}
