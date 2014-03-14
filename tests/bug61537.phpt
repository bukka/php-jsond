--TEST--
Bug #61537 (jsond_encode() incorrectly truncates/discards information)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
$invalid_utf8 = "\x9f";

var_dump(jsond_encode($invalid_utf8));
var_dump(jsond_last_error(), jsond_last_error_msg());

var_dump(jsond_encode($invalid_utf8, JSOND_PARTIAL_OUTPUT_ON_ERROR));
var_dump(jsond_last_error(), jsond_last_error_msg());

echo "\n";

$invalid_utf8 = "an invalid sequen\xce in the middle of a string";

var_dump(jsond_encode($invalid_utf8));
var_dump(jsond_last_error(), jsond_last_error_msg());

var_dump(jsond_encode($invalid_utf8, JSOND_PARTIAL_OUTPUT_ON_ERROR));
var_dump(jsond_last_error(), jsond_last_error_msg());

?>
--EXPECTF--
bool(false)
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
string(4) "null"
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"

bool(false)
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
string(4) "null"
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
