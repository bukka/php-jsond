--TEST--
Bug #41567 (jsond_encode() double conversion is inconsistent with PHP)
--EXTENSIONS--
jsond
--INI--
serialize_precision=-1
--FILE--
<?php
require_once "bootstrap.inc";

$a = jsond_encode(123456789.12345);
var_dump(jsond_decode($a));

echo "Done\n";
?>
--EXPECT--
float(123456789.12345)
Done
