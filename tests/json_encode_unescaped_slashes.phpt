--TEST--
jsond_decode() tests
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
var_dump(jsond_encode('a/b'));
var_dump(jsond_encode('a/b', JSOND_UNESCAPED_SLASHES));
?>
--EXPECT--
string(6) ""a\/b""
string(5) ""a/b""
