--TEST--
jsond_encode() & endless loop - 1
--SKIPIF--
<?php
if (!extension_loaded("jsond"))
	print "skip";
if (PHP_VERSION_ID < 50400)
	print "skip Only for 5.4+";
?>
--FILE--
<?php

$a = array();
$a[] = &$a;

var_dump($a);

echo "\n";

var_dump(jsond_encode($a));
var_dump(jsond_last_error(), jsond_last_error_msg());

echo "\n";

var_dump(jsond_encode($a, JSOND_PARTIAL_OUTPUT_ON_ERROR));
var_dump(jsond_last_error(), jsond_last_error_msg());

echo "Done\n";
?>
--EXPECTF--	
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    *RECURSION*
  }
}

bool(false)
int(6)
string(%d) "Recursion detected"

string(8) "[[null]]"
int(6)
string(%d) "Recursion detected"
Done
