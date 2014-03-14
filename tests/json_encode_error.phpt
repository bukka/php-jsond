--TEST--
Test jsond_encode() function : error conditions
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
 	die('skip JSON extension not available in this build');
}	 
?>
--FILE--
<?php
/* Prototype  : string jsond_encode  ( mixed $value  [, int $options=0  ] )
 * Description: Returns the JSON representation of a value
 * Source code: ext/jsond/php_jsond.c
 * Alias to functions: 
 */

echo "*** Testing jsond_encode() : error conditions ***\n";

echo "\n-- Testing jsond_encode() function with no arguments --\n";
var_dump( jsond_encode() );

echo "\n-- Testing jsond_encode() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( jsond_encode("abc", 0, $extra_arg) );

?>
===Done===
--EXPECTF--
*** Testing jsond_encode() : error conditions ***

-- Testing jsond_encode() function with no arguments --

Warning: jsond_encode() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing jsond_encode() function with more than expected no. of arguments --
string(5) ""abc""
===Done===
