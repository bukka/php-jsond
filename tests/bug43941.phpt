--TEST--
Bug #43941 (json_encode() invalid UTF-8)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_encode("abc"));
var_dump($jsond_encode("ab\xE0"));
var_dump($jsond_encode("ab\xE0", jsond_constant('PARTIAL_OUTPUT_ON_ERROR')));
var_dump($jsond_encode(
    array("ab\xE0", "ab\xE0c", "abc"),
    jsond_constant('PARTIAL_OUTPUT_ON_ERROR')));

echo "Done\n";
?>
--EXPECTF--
string(5) ""abc""
bool(false)
string(4) "null"
string(17) "[null,null,"abc"]"
Done
