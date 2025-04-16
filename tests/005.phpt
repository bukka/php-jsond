--TEST--
jsond_encode() & endless loop - 3
--EXTENSIONS--
jsond
--FILE--
<?php
require_once "bootstrap.inc";

$a = array();
$a[] = $a;

var_dump($a);
var_dump(jsond_encode($a));

echo "Done\n";
?>
--EXPECTF--
array(1) {
  [0]=>
  array(0) {
  }
}
string(4) "[[]]"
Done
