--TEST--
Bug #71835 (json_encode sometimes incorrectly detects recursion with JsonSerializable)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

if ($jsond_prefix === 'jsond') {
	class SomeClass implements JsondSerializable {
		public function jsonSerialize() {
			return [get_object_vars($this)];
		}
	}
	class SomeClass2 implements JsondSerializable {
		public function jsonSerialize() {
			return [(array)$this];
		}
	}
} else {
	class SomeClass implements JsonSerializable {
		public function jsonSerialize() {
			return [get_object_vars($this)];
		}
	}
	class SomeClass2 implements JsonSerializable {
		public function jsonSerialize() {
			return [(array)$this];
		}
	}
}

$class = new SomeClass;
$arr = [$class];
var_dump($jsond_encode($arr));

$class = new SomeClass2;
$arr = [$class];
var_dump($jsond_encode($arr));
?>
--EXPECT--
string(6) "[[[]]]"
string(6) "[[[]]]"
