--TEST--
Bug #73113 (Segfault with throwing JsondSer_ializable) - also test that the custom exception is not wrapped
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

class JsondSer_ializableObject implements \JsondSerializable {
    public function jsonSerialize(): mixed {
        throw new \Exception('This error is expected');
    }
}

$obj = new JsondSer_ializableObject();
try {
    echo jsond_encode($obj);
} catch (\Exception $e) {
    echo $e->getMessage();
}
?>
--EXPECTF--
This error is expected