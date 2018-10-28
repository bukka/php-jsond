--TEST--
Bug #61978 (Object recursion not detected for classes that implement JsonSerializable)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

class JsonTest1 {
    public $test;
    public $me;
    public function __construct() {
        $this->test = '123';
        $this->me  = $this;
    }
}

if ($jsond_prefix === 'jsond') {
    class JsonTest2 implements JsondSerializable {
        public $test;
        public function __construct() {
            $this->test = '123';
        }
        public function jsonSerialize() {
            return array(
                'test' => $this->test,
                'me'   => $this
            );
        }
    }
} else {
    class JsonTest2 implements JsonSerializable {
        public $test;
        public function __construct() {
            $this->test = '123';
        }
        public function jsonSerialize() {
            return array(
                'test' => $this->test,
                'me'   => $this
            );
        }

    }
}


$obj1 = new JsonTest1();
var_dump($jsond_encode($obj1, jsond_constant('PARTIAL_OUTPUT_ON_ERROR')));

echo "==\n";

$obj2 = new JsonTest2();
var_dump($jsond_encode($obj2, jsond_constant('PARTIAL_OUTPUT_ON_ERROR')));

?>
--EXPECTF--
string(24) "{"test":"123","me":null}"
==
string(24) "{"test":"123","me":null}"
