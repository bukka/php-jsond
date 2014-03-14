--TEST--
Bug #55543 (jsond_encode() with JSOND_NUMERIC_CHECK & numeric string properties)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
$a = new stdClass;
$a->{"1"} = "5";

var_dump(jsond_encode($a, JSOND_NUMERIC_CHECK));
?>
--EXPECT--
string(7) "{"1":5}"
