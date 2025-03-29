--TEST--
jsond_encode() invalid UTF8
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
	die('skip JSON extension not available in this build');
}
?>
--FILE--
<?php
require_once "bootstrap.inc";

function jsond_encode_invalid_utf8($str) {
	var_dump(jsond_encode($str));
	var_dump(jsond_encode($str,  JSOND_INVALID_UTF8_IGNORE));
	var_dump(jsond_encode($str,  JSOND_INVALID_UTF8_SUBSTITUTE));
	var_dump(jsond_encode($str,  JSOND_UNESCAPED_UNICODE));
	var_dump(bin2hex(jsond_encode($str,  JSOND_UNESCAPED_UNICODE |  JSOND_INVALID_UTF8_SUBSTITUTE)));
}

jsond_encode_invalid_utf8("\x61\xb0\x62");
jsond_encode_invalid_utf8("\x61\xf0\x9d\x85\x41");
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
