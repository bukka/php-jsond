--TEST--
Bug #62010 (jsond_decode produces invalid byte-sequences)
--EXTENSIONS--
jsond
--FILE--
<?php
require_once "bootstrap.inc";

var_dump(jsond_decode('"\ud834"'));
var_dump(jsond_last_error() === JSOND_ERROR_UTF16);
var_dump(jsond_last_error_msg());
?>
--EXPECTF--
NULL
bool(true)
string(50) "Single unpaired UTF-16 surrogate in unicode escape"
