--TEST--
Bug #55543 (json_encode() with JSOND_NUMERIC_CHECK & numeric string properties)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

$a = new stdClass;
$a->{"1"} = "5";

var_dump($jsond_encode($a, jsond_constant('NUMERIC_CHECK')));
?>
--EXPECT--
string(7) "{"1":5}"
