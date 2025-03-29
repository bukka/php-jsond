--TEST--
jsond_decode() invalid UTF8
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
    die('skip JSON extension not available in this build');
}
?>
--FILE--
<?php
require_once "bootstrap.inc";

function jsond_decode_invalid_utf8($str) {
    var_dump(jsond_decode($str));
    var_dump(jsond_decode($str, true, 512, JSOND_INVALID_UTF8_IGNORE));
    $json = jsond_decode($str, true, 512, JSOND_INVALID_UTF8_SUBSTITUTE);
    if (is_array($json)) {
        var_dump(array_map(function($item) { return bin2hex($item); }, $json));
    } else {
        var_dump(bin2hex($json));
    }
}
jsond_decode_invalid_utf8("\"a\xb0b\"");
jsond_decode_invalid_utf8("\"a\xd0\xf2b\"");
jsond_decode_invalid_utf8("\"\x61\xf0\x80\x80\x41\"");
jsond_decode_invalid_utf8("[\"\xc1\xc1\",\"a\"]");
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
NULL
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(1) "a"
}
array(2) {
  [0]=>
  string(12) "efbfbdefbfbd"
  [1]=>
  string(2) "61"
}