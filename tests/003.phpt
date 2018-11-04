--TEST--
json_encode() & endless loop - 1
--SKIPIF--
<?php
if (!extension_loaded("jsond"))
	print "skip";
?>
--FILE--
<?php
require_once "bootstrap.inc";

$a = array();
$a[] = &$a;

var_dump($a);

echo "\n";

var_dump($jsond_encode($a));
var_dump($jsond_last_error(), $jsond_last_error_msg());

echo "\n";


$output = $jsond_encode($a, jsond_constant('PARTIAL_OUTPUT_ON_ERROR'));
var_dump($output === (version_compare(PHP_VERSION, '7.0.0') >= 0 ? '[null]': '[[null]]'));
var_dump($jsond_last_error(), $jsond_last_error_msg());

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

bool(true)
int(6)
string(%d) "Recursion detected"
Done
