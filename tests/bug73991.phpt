--TEST--
Bug #73991 (Allow JSON_OBJECT_AS_ARRAY to have an effect)
--FILE--
<?php
require_once "bootstrap.inc";

$json = '{"foo":"bar"}';

var_dump($jsond_decode($json, false));
var_dump($jsond_decode($json, true));
var_dump($jsond_decode($json, null, 512, 0));
var_dump($jsond_decode($json, null, 512, jsond_constant('OBJECT_AS_ARRAY')));
?>
--EXPECTF--
object(stdClass)#%d (1) {
  ["foo"]=>
  string(3) "bar"
}
array(1) {
  ["foo"]=>
  string(3) "bar"
}
object(stdClass)#%d (1) {
  ["foo"]=>
  string(3) "bar"
}
array(1) {
  ["foo"]=>
  string(3) "bar"
}
