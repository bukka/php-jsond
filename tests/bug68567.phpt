--TEST--
Bug #68567 JSON_PARTIAL_OUTPUT_ON_ERROR can result in JSON with null key
--EXTENSIONS--
jsond
--FILE--
<?php
require_once "bootstrap.inc";

var_dump(jsond_encode(array("\x80" => 1), JSOND_PARTIAL_OUTPUT_ON_ERROR));

?>
--EXPECTF--
string(6) "{"":1}"
