--TEST--
Bug #77843: Use after free with json serializer
--SKIPIF--
<?php
if (!extension_loaded('jsond')) die('skip');
?>
--FILE--
<?php

class X implements JsondSerializable {
    public $prop = "value";
    public function jsonSerialize() {
        global $arr;
        unset($arr[0]);
        var_dump($this);
        return $this;
    }
}

$arr = [new X()];
var_dump(jsond_encode([&$arr]));

?>
--EXPECT--
object(X)#1 (1) {
  ["prop"]=>
  string(5) "value"
}
string(20) "[[{"prop":"value"}]]"