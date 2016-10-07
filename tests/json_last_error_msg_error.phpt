--TEST--
json_last_error_msg() failures
--SKIPIF--
<?php !extension_loaded('jsond') && die('skip json extension not available') ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_last_error_msg());
var_dump($jsond_last_error_msg(TRUE));
var_dump($jsond_last_error_msg('some', 4, 'args', 'here'));

?>
--EXPECTF--
string(8) "No error"

Warning: %s expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: %s expects exactly 0 parameters, 4 given in %s on line %d
NULL

