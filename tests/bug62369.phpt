--TEST--
FR #62369 (Segfault on json_encode(deeply_nested_array)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

$array = array();
for ($i=0; $i<550; $i++) {
    $array = array($array);
}

$jsond_encode($array, 0, 551);
if ($jsond_last_error() === jsond_constant('ERROR_NONE')) {
    echo 'OK'.PHP_EOL;
}

$jsond_encode($array, 0, 540);
if ($jsond_last_error() === jsond_constant('ERROR_DEPTH')) {
    echo 'ERROR'.PHP_EOL;
}
?>
--EXPECTF--
OK
ERROR
