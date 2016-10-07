--TEST--
Bug #68546 (json_decode() Fatal error: Cannot access property started with '\0')
--SKIPIF--
<?php

if (!extension_loaded('jsond')) die('skip');
?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_decode('{"key": {"\u0000": "aa"}}'));
var_dump($jsond_last_error() === jsond_constant('ERROR_INVALID_PROPERTY_NAME'));
var_dump($jsond_decode('[{"key1": 0, "\u0000": 1}]'));
var_dump($jsond_last_error() === jsond_constant('ERROR_INVALID_PROPERTY_NAME'));
var_dump($jsond_last_error_msg());

echo "Done\n";
?>
--EXPECT--
NULL
bool(true)
NULL
bool(true)
string(36) "The decoded property name is invalid"
Done
