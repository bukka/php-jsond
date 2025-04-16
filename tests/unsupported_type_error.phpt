--TEST--
An error is thrown when an unsupported type is encoded
--EXTENSIONS--
jsond
--FILE--
<?php
require_once "bootstrap.inc";

$resource = fopen(__FILE__, "r");

var_dump($resource);

var_dump(jsond_encode($resource));
var_dump(jsond_last_error(), jsond_last_error_msg());

var_dump(jsond_encode($resource, JSOND_PARTIAL_OUTPUT_ON_ERROR));
var_dump(jsond_last_error(), jsond_last_error_msg());

?>
--EXPECTF--
resource(%d) of type (stream)
bool(false)
int(8)
string(21) "Type is not supported"
string(4) "null"
int(8)
string(21) "Type is not supported"
