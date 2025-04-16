--TEST--
Bug #40503 (jsond_encode() value corruption on 32bit systems with overflown values)
--EXTENSIONS--
jsond
--INI--
serialize_precision=-1
--FILE--
<?php
require_once "bootstrap.inc";

function show_eq($x,$y) {
    echo "$x ". ($x==$y ? "==" : "!=") ." $y\n";
}

$value = 0x7FFFFFFF; #2147483647;
show_eq("$value", jsond_encode($value));
$value++;
show_eq("$value", jsond_encode($value));

?>
--EXPECT--
2147483647 == 2147483647
2147483648 == 2147483648
