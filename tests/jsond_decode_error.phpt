--TEST--
Test jsond_decode() function : error conditions
--EXTENSIONS--
jsond
--FILE--
<?php
require_once "bootstrap.inc";

echo "*** Testing jsond_decode() : error conditions ***\n";

echo "\n-- Testing jsond_decode() function with no arguments --\n";
jsond_expect_argument_count_error(
    function() { jsond_decode(); },
    'jsond_decode() expects at least 1 argument, 0 given',
    'Warning: jsond_decode() expects at least 1 parameter, 0 given in file on line 0'
);

echo "\n-- Testing jsond_decode() function with more than expected no. of arguments --\n";

jsond_expect_argument_count_error(
    function() {
        $extra_arg = 10;
        jsond_decode('"abc"', true, 512, 0, null, $extra_arg);
    },
    'jsond_decode() expects at most 5 arguments, 6 given',
    'Warning: jsond_decode() expects at most 5 parameters, 6 given in file on line 0'
);

echo "\n-- Testing jsond_decode() function with depth below 0 --\n";


try {
    var_dump(jsond_decode('"abc"', true, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(jsond_decode('"abc"', true, 1000, 0, 'schema'));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
*** Testing %s : error conditions ***

-- Testing jsond_decode() function with no arguments --

Warning: %s expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing jsond_decode() function with more than expected no. of arguments --

Warning: jsond_decode() expects at most 5 parameters, 6 given in %s on line %d
NULL

-- Testing jsond_decode() function with depth below 0 --
jsond_decode(): Argument #3 ($depth) must be greater than zero
jsond_decode(): Argument #5 ($schema) must be of type JsondSchema, string given
