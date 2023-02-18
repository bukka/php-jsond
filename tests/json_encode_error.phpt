--TEST--
Test json_encode() function : error conditions
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
 	die('skip JSON extension not available in this build');
}
?>
--FILE--
<?php
require_once "bootstrap.inc";

echo "*** Testing json_encode() : error conditions ***\n";

echo "\n-- Testing json_encode() function with no arguments --\n";
jsond_expect_argument_count_error(
    function() use ($jsond_encode) { $jsond_encode(); },
    'jsond_encode() expects at least 1 argument, 0 given',
    'Warning: jsond_encode() expects at least 1 parameter, 0 given in file on line 0'
);

echo "\n-- Testing json_encode() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump($jsond_encode("abc", 0, $extra_arg));

?>
===Done===
--EXPECTF--
*** Testing json_encode() : error conditions ***

-- Testing json_encode() function with no arguments --

Warning: %s expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing json_encode() function with more than expected no. of arguments --
string(5) ""abc""
===Done===
