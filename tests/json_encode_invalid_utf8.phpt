--TEST--
json_encode() invalid UTF8
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
	die('skip JSON extension not available in this build');
}
?>
--FILE--
<?php
require_once "bootstrap.inc";

function json_encode_invalid_utf8($str) {
	global $jsond_encode;
	var_dump($jsond_encode($str));
	var_dump($jsond_encode($str,  jsond_constant('INVALID_UTF8_IGNORE')));
	var_dump($jsond_encode($str,  jsond_constant('INVALID_UTF8_SUBSTITUTE')));
	var_dump($jsond_encode($str,  jsond_constant('UNESCAPED_UNICODE')));
	var_dump(bin2hex($jsond_encode($str,  jsond_constant('UNESCAPED_UNICODE') |  jsond_constant('INVALID_UTF8_SUBSTITUTE'))));
}

json_encode_invalid_utf8("\x61\xb0\x62");
json_encode_invalid_utf8("\x61\xf0\x9d\x85\x41");
echo "Done\n";
?>
--EXPECT--
bool(false)
string(4) ""ab""
string(10) ""a\ufffdb""
bool(false)
string(14) "2261efbfbd6222"
bool(false)
string(4) ""aA""
string(10) ""a\ufffdA""
bool(false)
string(14) "2261efbfbd4122"
Done
