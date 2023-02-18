--TEST--
Bug #73113 (Segfault with throwing JsonSerializable) - also test that the custom exception is not wrapped
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

if ($jsond_prefix === 'jsond') {
    class JsonSerializableObject implements \JsondSerializable {
        public function jsonSerialize() {
            throw new \Exception('This error is expected');
        }
    }
} else {
    class JsonSerializableObject implements \JsonSerializable {
        public function jsonSerialize() {
            throw new \Exception('This error is expected');
        }
    }
}

$obj = new JsonSerializableObject();
try {
	echo $jsond_encode($obj);
} catch (\Exception $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
This error is expected