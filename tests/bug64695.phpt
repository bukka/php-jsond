--TEST--
Bug #63737 JSOND_NUMERIC_CHECK has issues with strings that are numbers plus the letter e
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
$t = array('test' => '123343e871700');
var_dump(jsond_encode($t, JSOND_NUMERIC_CHECK));

echo "Done\n";
?>
--EXPECT--
string(24) "{"test":"123343e871700"}"
Done
