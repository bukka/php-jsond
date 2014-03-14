--TEST--
Test jsond_decode() function : error conditions
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
 	die('skip JSON extension not available in this build');
}	 
?>
--FILE--
<?php
/* Prototype  : mixed jsond_decode  ( string $jsond  [, bool $assoc=false  [, int $depth=512  ]] )
 * Description: Decodes a JSON string
 * Source code: ext/jsond/php_jsond.c
 * Alias to functions: 
 */
echo "*** Testing jsond_decode() : error conditions ***\n";

echo "\n-- Testing jsond_decode() function with no arguments --\n";
var_dump( jsond_decode() );

echo "\n-- Testing jsond_decode() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( jsond_decode('"abc"', TRUE, 512, 0, $extra_arg) );

?>
===Done===
--EXPECTF--
*** Testing jsond_decode() : error conditions ***

-- Testing jsond_decode() function with no arguments --

Warning: jsond_decode() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing jsond_decode() function with more than expected no. of arguments --

Warning: jsond_decode() expects at most 4 parameters, 5 given in %s on line %d
NULL
===Done===
