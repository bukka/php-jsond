--TEST--
FR #62369 (Segfault on jsond_encode(deeply_nested_array)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php

$array = array();
for ($i=0; $i<550; $i++) {
    $array = array($array);
}

jsond_encode($array, 0, 551);
switch (jsond_last_error()) {
    case JSOND_ERROR_NONE:
        echo 'OK'.PHP_EOL;
    break;
    case JSOND_ERROR_DEPTH:
        echo 'ERROR'.PHP_EOL;
    break;
}

jsond_encode($array, 0, 540);
switch (jsond_last_error()) {
    case JSOND_ERROR_NONE:
        echo 'OK'.PHP_EOL;
    break;
    case JSOND_ERROR_DEPTH:
        echo 'ERROR'.PHP_EOL;
    break;
}
--EXPECTF--
OK
ERROR
