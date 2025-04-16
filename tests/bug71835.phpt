--TEST--
Bug #71835 (jsond_encode sometimes incorrectly detects recursion with JsondSer_ializable)
--EXTENSIONS--
jsond
--FILE--
<?php
require_once "bootstrap.inc";

class SomeClass implements JsondSerializable {
    public function jsonSerialize(): mixed {
        return [get_object_vars($this)];
    }
}
class SomeClass2 implements JsondSerializable {
    public function jsonSerialize(): mixed {
        return [(array)$this];
    }
}

$class = new SomeClass;
$arr = [$class];
var_dump(jsond_encode($arr));

$class = new SomeClass2;
$arr = [$class];
var_dump(jsond_encode($arr));
?>
--EXPECT--
string(6) "[[[]]]"
string(6) "[[[]]]"
