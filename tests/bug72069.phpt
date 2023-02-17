--TEST--
Bug #72069 (Behavior \JsonSerializable different from json_encode)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

if ($jsond_prefix === 'jsond') {
	class A implements \JsondSerializable
	{
		function jsonSerialize()
		{
			global $jsond_decode;
			return ['end' => $jsond_decode("null", true)];
		}
	}
} else {
	class A implements \JsonSerializable
	{
		function jsonSerialize()
		{
			global $jsond_decode;
			return ['end' => $jsond_decode("null", true)];
		}
	}
}

$result = $jsond_encode(['end' => $jsond_decode("null", true)]);
var_dump($result);
$a = new A();
$toJsonData = $a->jsonSerialize();
$result = $jsond_encode($a);
var_dump($result);

$result = $jsond_encode($toJsonData);
var_dump($result);
?>
--EXPECT--
string(12) "{"end":null}"
string(12) "{"end":null}"
string(12) "{"end":null}"
