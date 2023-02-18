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

echo "*** Testing json_decode() : error conditions ***\n";

echo "\n-- Testing json_decode() function with no arguments --\n";
jsond_expect_argument_count_error(
    function() use ($jsond_decode) { $jsond_decode(); },
    'jsond_decode() expects at least 1 argument, 0 given',
    'Warning: jsond_decode() expects at least 1 parameter, 0 given in file on line 0'
);

echo "\n-- Testing json_decode() function with more than expected no. of arguments --\n";

jsond_expect_argument_count_error(
    function() use ($jsond_decode) {
        $extra_arg = 10;
        $jsond_decode('"abc"', TRUE, 512, 0, $extra_arg);
    },
    'jsond_decode() expects at most 4 arguments, 5 given',
    'Warning: jsond_decode() expects at most 4 parameters, 5 given in file on line 0'
);

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
