--TEST--
Bug #68992 (jsond_encode stacks exceptions thrown by JsondSer_ializable classes)
--SKIPIF--
<?php
if (!extension_loaded('jsond')) die('skip');
?>
--FILE--
<?php
require_once "bootstrap.inc";

class MyClass implements JsondSerializable {
    public function jsonSerialize(): mixed {
        throw new Exception('Not implemented!');
    }
}

$classes = [];
for ($i = 0; $i < 5; $i++) {
    $classes[] = new MyClass();
}

try {
    jsond_encode($classes);
} catch(Exception $e) {
    do {
        printf("%s (%d) [%s]\n", $e->getMessage(), $e->getCode(), get_class($e));
    } while ($e = $e->getPrevious());
}
?>
--EXPECT--
Not implemented! (0) [Exception]