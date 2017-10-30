--TEST--
json_encode() tests for U+2028, U+2029
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_encode(array("a\xC3\xA1b")));
var_dump($jsond_encode(array("a\xC3\xA1b"), jsond_constant('UNESCAPED_UNICODE')));
var_dump($jsond_encode("a\xE2\x80\xA7b"));
var_dump($jsond_encode("a\xE2\x80\xA7b", jsond_constant('UNESCAPED_UNICODE')));
var_dump($jsond_encode("a\xE2\x80\xA8b"));
var_dump($jsond_encode("a\xE2\x80\xA8b", jsond_constant('UNESCAPED_UNICODE')));
var_dump($jsond_encode("a\xE2\x80\xA8b", jsond_constant('UNESCAPED_LINE_TERMINATORS')));
var_dump($jsond_encode("a\xE2\x80\xA8b", jsond_constant('UNESCAPED_UNICODE') | jsond_constant('UNESCAPED_LINE_TERMINATORS')));
var_dump($jsond_encode("a\xE2\x80\xA9b"));
var_dump($jsond_encode("a\xE2\x80\xA9b", jsond_constant('UNESCAPED_UNICODE')));
var_dump($jsond_encode("a\xE2\x80\xA9b", jsond_constant('UNESCAPED_LINE_TERMINATORS')));
var_dump($jsond_encode("a\xE2\x80\xA9b", jsond_constant('UNESCAPED_UNICODE') | jsond_constant('UNESCAPED_LINE_TERMINATORS')));
var_dump($jsond_encode("a\xE2\x80\xAAb"));
var_dump($jsond_encode("a\xE2\x80\xAAb", jsond_constant('UNESCAPED_UNICODE')));
?>
--EXPECT--
string(12) "["a\u00e1b"]"
string(8) "["aáb"]"
string(10) ""a\u2027b""
string(7) ""a‧b""
string(10) ""a\u2028b""
string(10) ""a\u2028b""
string(10) ""a\u2028b""
string(7) ""a b""
string(10) ""a\u2029b""
string(10) ""a\u2029b""
string(10) ""a\u2029b""
string(7) ""a b""
string(10) ""a\u202ab""
string(7) ""a‪b""
