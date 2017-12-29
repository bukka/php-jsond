--TEST--
Bug #40503 (json_encode() value corruption on 32bit systems with overflown values)
--INI--
serialize_precision=14
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

function show_eq($x,$y) {
	echo "$x ". ($x==$y ? "==" : "!=") ." $y\n";
}

$value = 0x7FFFFFFF; #2147483647;
show_eq("$value", $jsond_encode($value));
$value++;
show_eq("$value", $jsond_encode($value));

?>
--EXPECT--
2147483647 == 2147483647
2147483648 == 2147483648
