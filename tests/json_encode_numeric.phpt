--TEST--
Test jsond_encode() function with numeric flag
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
	die('skip JSON extension not available in this build');
}	 
?>
--FILE--
<?php
var_dump(
	jsond_encode("1", JSOND_NUMERIC_CHECK),
	jsond_encode("9.4324", JSOND_NUMERIC_CHECK),
	jsond_encode(array("122321", "3232595.33423"), JSOND_NUMERIC_CHECK),
	jsond_encode("1"),
	jsond_encode("9.4324"),
	jsond_encode(array("122321", "3232595.33423"))
);
?>
--EXPECT--
string(1) "1"
string(6) "9.4324"
string(22) "[122321,3232595.33423]"
string(3) ""1""
string(8) ""9.4324""
string(26) "["122321","3232595.33423"]"
