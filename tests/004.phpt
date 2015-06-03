--TEST--
jsond_encode() & endless loop - 2
--SKIPIF--
<?php
if (!extension_loaded("jsond"))
	print "skip";
?>
--FILE--
<?php
require_once "bootstrap.inc";

$a = new stdclass;
$a->prop = $a;

var_dump($a);

echo "\n";

var_dump($jsond_encode($a));
var_dump($jsond_last_error(), $jsond_last_error_msg());

echo "\n";

var_dump($jsond_encode($a, jsond_constant('PARTIAL_OUTPUT_ON_ERROR')));
var_dump($jsond_last_error(), $jsond_last_error_msg());

echo "Done\n";
?>
--EXPECTF--
object(stdClass)#%d (1) {
  ["prop"]=>
  *RECURSION*
}

bool(false)
int(6)
string(%d) "Recursion detected"

string(22) "{"prop":{"prop":null}}"
int(6)
string(%d) "Recursion detected"
Done
