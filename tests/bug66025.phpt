--TEST--
Bug #66025 (Indent wrong when jsond_encode() called from jsonSerialize function)
--EXTENSIONS--
jsond
--FILE--
<?php
require_once "bootstrap.inc";

class JsondSer_ializableObject implements \JsondSerializable {
    public function jsonSerialize(): mixed {
        return jsond_encode([1], JSOND_PRETTY_PRINT);
    }
}

echo jsond_encode([new JsondSer_ializableObject]), "\n";
?>
--EXPECT--
["[\n    1\n]"]