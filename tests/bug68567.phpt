--TEST--
Bug #68567 JSON_PARTIAL_OUTPUT_ON_ERROR can result in JSON with null key
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_encode(array("\x80" => 1), jsond_constant('PARTIAL_OUTPUT_ON_ERROR')));

?>
===DONE===
--EXPECTF--
string(6) "{"":1}"
===DONE===
