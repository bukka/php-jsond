--TEST--
jsond_decode() with large integers
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

$jsond = '{"largenum":123456789012345678901234567890}';
$x = $jsond_decode($jsond);
var_dump($x->largenum);
$x = $jsond_decode($jsond, false, 512, jsond_constant('BIGINT_AS_STRING'));
var_dump($x->largenum);
echo "Done\n";
?>
--EXPECT--
float(1.2345678901235E+29)
string(30) "123456789012345678901234567890"
Done
