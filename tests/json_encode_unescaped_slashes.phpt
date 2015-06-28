--TEST--
json_decode() unescaped slashes test
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_encode('a/b'));
var_dump($jsond_encode('a/b', jsond_constant('UNESCAPED_SLASHES')));
?>
--EXPECT--
string(6) ""a\/b""
string(5) ""a/b""
