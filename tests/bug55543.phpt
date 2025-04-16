--TEST--
Bug #55543 (jsond_encode() with JSOND_NUMERIC_CHECK & numeric string properties)
--EXTENSIONS--
jsond
--FILE--
<?php
require_once "bootstrap.inc";

$a = new stdClass;
$a->{"1"} = "5";

var_dump(jsond_encode($a, JSOND_NUMERIC_CHECK));
?>
--EXPECT--
string(7) "{"1":5}"
