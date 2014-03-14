--TEST--
Bug #41567 (jsond_encode() double conversion is inconsistent with PHP)
--INI--
precision=14
--SKIPIF--
<?php if (!extension_loaded('jsond')) print 'skip'; ?>
--FILE--
<?php

$a = jsond_encode(123456789.12345);
var_dump(jsond_decode($a));

echo "Done\n";
?>
--EXPECT--	
float(123456789.12345)
Done
