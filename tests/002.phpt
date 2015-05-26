--TEST--
json_encode() tests
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_encode(""));
var_dump($jsond_encode(NULL));
var_dump($jsond_encode(TRUE));

var_dump($jsond_encode(array(""=>"")));
var_dump($jsond_encode(array(array(1))));
var_dump($jsond_encode(array()));

var_dump($jsond_encode(array(""=>""), jsond_constant('FORCE_OBJECT')));
var_dump($jsond_encode(array(array(1)), jsond_constant('FORCE_OBJECT')));
var_dump($jsond_encode(array(), jsond_constant('FORCE_OBJECT')));

var_dump($jsond_encode(1));
var_dump($jsond_encode("руссиш"));


echo "Done\n";
?>
--EXPECTF--
string(2) """"
string(4) "null"
string(4) "true"
string(7) "{"":""}"
string(5) "[[1]]"
string(2) "[]"
string(7) "{"":""}"
string(13) "{"0":{"0":1}}"
string(2) "{}"
string(1) "1"
string(38) ""\u0440\u0443\u0441\u0441\u0438\u0448""
Done
