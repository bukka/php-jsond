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
require_once "bootstrap.inc";

echo "*** Testing jsond_encode() : error conditions ***\n";

echo "\n-- Testing jsond_encode() function with no arguments --\n";
jsond_expect_argument_count_error(
    function() { jsond_encode(); },
    'jsond_encode() expects at least 1 argument, 0 given',
    'Warning: jsond_encode() expects at least 1 parameter, 0 given in file on line 0'
);

echo "\n-- Testing jsond_encode() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump(jsond_encode("abc", 0, $extra_arg));

?>
--EXPECTF--
*** Testing jsond_encode() : error conditions ***

-- Testing jsond_encode() function with no arguments --

Warning: %s expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing jsond_encode() function with more than expected no. of arguments --
string(5) ""abc""
