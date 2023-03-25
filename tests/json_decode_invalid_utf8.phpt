--TEST--
json_decode() invalid UTF8
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
	die('skip JSON extension not available in this build');
}
?>
--FILE--
<?php
require_once "bootstrap.inc";

function json_decode_invalid_utf8($str) {
	global $jsond_decode;
    var_dump($jsond_decode($str));
	var_dump($jsond_decode($str, true, 512, jsond_constant('INVALID_UTF8_IGNORE')));
	var_dump(bin2hex($jsond_decode($str, true, 512, jsond_constant('INVALID_UTF8_SUBSTITUTE'))));
}
json_decode_invalid_utf8("\"a\xb0b\"");
json_decode_invalid_utf8("\"a\xd0\xf2b\"");
json_decode_invalid_utf8("\"\x61\xf0\x80\x80\x41\"");
echo "Done\n";
?>
--EXPECT--
NULL
string(2) "ab"
string(10) "61efbfbd62"
NULL
string(2) "ab"
string(16) "61efbfbdefbfbd62"
NULL
string(2) "aA"
string(22) "61efbfbdefbfbdefbfbd41"
Done