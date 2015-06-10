--TEST--
bug #53946 (json_encode() with JSOND_UNESCAPED_UNICODE)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_encode(
    "latin 1234 -/    russian мама мыла раму  specialchars \x02   \x08 \n   U+1D11E >𝄞<"));
var_dump($jsond_encode(
    "latin 1234 -/    russian мама мыла раму  specialchars \x02   \x08 \n   U+1D11E >𝄞<",
    jsond_constant('UNESCAPED_UNICODE')));
var_dump($jsond_encode("ab\xE0"));
var_dump($jsond_encode("ab\xE0", jsond_constant('UNESCAPED_UNICODE')));
?>
--EXPECTF--
string(156) ""latin 1234 -\/    russian \u043c\u0430\u043c\u0430 \u043c\u044b\u043b\u0430 \u0440\u0430\u043c\u0443  specialchars \u0002   \b \n   U+1D11E >\ud834\udd1e<""
string(100) ""latin 1234 -\/    russian мама мыла раму  specialchars \u0002   \b \n   U+1D11E >𝄞<""
bool(false)
bool(false)
