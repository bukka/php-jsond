--TEST--
Bug #41567 (json_encode() double conversion is inconsistent with PHP)
--INI--
precision=14
--SKIPIF--
<?php if (!extension_loaded('jsond')) print 'skip'; ?>
--FILE--
<?php
require_once "bootstrap.inc";

$a = $jsond_encode(123456789.12345);
var_dump($jsond_decode($a));

echo "Done\n";
?>
--EXPECT--
float(123456789.12345)
Done
