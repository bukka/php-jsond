--TEST--
Test json_decode() function : error conditions
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
 	die('skip JSON extension not available in this build');
}
?>
--FILE--
<?php
require_once "bootstrap.inc";

echo "*** Testing jsond_decode() : error conditions ***\n";

echo "\n-- Testing json_decode() function with no arguments --\n";
var_dump( $jsond_decode() );

echo "\n-- Testing json_decode() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( $jsond_decode('"abc"', TRUE, 512, 0, $extra_arg) );

?>
===Done===
--EXPECTF--
*** Testing %s : error conditions ***

-- Testing json_decode() function with no arguments --

Warning: %s expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing json_decode() function with more than expected no. of arguments --

Warning: %s expects at most 4 parameters, 5 given in %s on line %d
NULL
===Done===
