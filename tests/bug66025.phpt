--TEST--
Bug #66025 (Indent wrong when json_encode() called from jsonSerialize function)
--SKIPIF--
<?php
if (!extension_loaded('jsond')) die('skip');
?>
--FILE--
<?php
require_once "bootstrap.inc";

if ($jsond_prefix === 'jsond') {
    class JsonSerializableObject implements \JsondSerializable {
        public function jsonSerialize() {
            return jsond_encode([1], jsond_constant('PRETTY_PRINT'));
        }
    }
} else {
    class JsonSerializableObject implements \JsonSerializable {
        public function jsonSerialize() {
            return json_encode([1], jsond_constant('PRETTY_PRINT'));
        }
    }
}

echo $jsond_encode([new JsonSerializableObject]), "\n";
?>
--EXPECT--
["[\n    1\n]"]