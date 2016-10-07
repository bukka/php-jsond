--TEST--
Bug #62010 (json_decode produces invalid byte-sequences)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_decode('"\ud834"'));
var_dump($jsond_last_error() === jsond_constant('ERROR_UTF16'));
var_dump($jsond_last_error_msg());
?>
--EXPECTF--
NULL
bool(true)
string(50) "Single unpaired UTF-16 surrogate in unicode escape"
