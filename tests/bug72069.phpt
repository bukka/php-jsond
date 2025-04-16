--TEST--
Bug #72069 (Behavior \JsondSer_ializable different from jsond_encode)
--EXTENSIONS--
jsond
--FILE--
<?php
require_once "bootstrap.inc";

class A implements \JsondSerializable
{
    function jsonSerialize(): mixed
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
