--TEST--
Test json_encode() function with numeric flag
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
	die('skip JSON extension not available in this build');
}
?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump(
	$jsond_encode("1", jsond_constant('NUMERIC_CHECK')),
	$jsond_encode("9.4324", jsond_constant('NUMERIC_CHECK')),
	$jsond_encode(array("122321", "3232595.33423"), jsond_constant('NUMERIC_CHECK')),
	$jsond_encode("1"),
	$jsond_encode("9.4324"),
	$jsond_encode(array("122321", "3232595.33423"))
);
?>
--EXPECT--
string(1) "1"
string(6) "9.4324"
string(22) "[122321,3232595.33423]"
string(3) ""1""
string(8) ""9.4324""
string(26) "["122321","3232595.33423"]"
