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

print_r($a);

echo "\n";

var_dump($jsond_encode($a));
var_dump($jsond_last_error(), $jsond_last_error_msg());

echo "\n";


$output = $jsond_encode($a, jsond_constant('PARTIAL_OUTPUT_ON_ERROR'));
var_dump($output === '[null]');
var_dump($jsond_last_error(), $jsond_last_error_msg());

echo "Done\n";
?>
--EXPECTF--
Array
(
    [0] => Array
 *RECURSION*
)

bool(false)
int(6)
string(%d) "Recursion detected"

bool(true)
int(6)
string(%d) "Recursion detected"
Done
