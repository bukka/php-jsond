--TEST--
json_last_error() failures
--SKIPIF--
<?php !extension_loaded('jsond') && die('skip json extension not available') ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_last_error());

jsond_expect_argument_count_error(
    function() use ($jsond_last_error) { $jsond_last_error(TRUE); },
    'jsond_last_error() expects exactly 0 arguments, 1 given',
    "Warning: jsond_last_error() expects exactly 0 parameters, 1 given in file on line 0",
);

jsond_expect_argument_count_error(
    function() use ($jsond_last_error) { $jsond_last_error('some', 4, 'args', 'here'); },
    'jsond_last_error() expects exactly 0 arguments, 4 given',
    "Warning: jsond_last_error() expects exactly 0 parameters, 4 given in file on line 0",
);

?>
--EXPECTF--
int(0)

Warning: %s expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: %s expects exactly 0 parameters, 4 given in %s on line %d
NULL

