--TEST--
Bug #68992 (json_encode stacks exceptions thrown by JsonSerializable classes)
--SKIPIF--
<?php
if (!extension_loaded('jsond')) die('skip');
?>
--FILE--
<?php
require_once "bootstrap.inc";

if ($jsond_prefix === 'jsond') {
    class MyClass implements JsondSerializable {
        public function jsonSerialize() {
            throw new Exception('Not implemented!');
        }
    }
} else {
    class MyClass implements JsonSerializable {
        public function jsonSerialize() {
            throw new Exception('Not implemented!');
        }
    }
}

$classes = [];
for ($i = 0; $i < 5; $i++) {
    $classes[] = new MyClass();
}

try {
    $jsond_encode($classes);
} catch(Exception $e) {
    do {
	    printf("%s (%d) [%s]\n", $e->getMessage(), $e->getCode(), get_class($e));
	} while ($e = $e->getPrevious());
}
?>
--EXPECT--
Failed calling MyClass::jsonSerialize() (0) [Exception]
Not implemented! (0) [Exception]