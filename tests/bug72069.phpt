--TEST--
Bug #72069 (Behavior \JsondSer_ializable different from jsond_encode)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

class A implements \JsondSerializable
{
	function jsonSerialize()
	{
		return ['end' => jsond_decode("null", true)];
	}
}

$result = jsond_encode(['end' => jsond_decode("null", true)]);
var_dump($result);
$a = new A();
$toJsonData = $a->jsonSerialize();
$result = jsond_encode($a);
var_dump($result);

$result = jsond_encode($toJsonData);
var_dump($result);
?>
--EXPECT--
string(12) "{"end":null}"
string(12) "{"end":null}"
string(12) "{"end":null}"
