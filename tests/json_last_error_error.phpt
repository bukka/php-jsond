--TEST--
json_last_error() failures
--SKIPIF--
<?php !extension_loaded('jsond') && die('skip json extension not available') ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_last_error());
var_dump($jsond_last_error(TRUE));
var_dump($jsond_last_error('some', 4, 'args', 'here'));


?>
--EXPECTF--
int(0)

Warning: %s expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: %s expects exactly 0 parameters, 4 given in %s on line %d
NULL

