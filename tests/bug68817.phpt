--TEST--
Bug #68817 (Null pointer deference)
--EXTENSIONS--
jsond
--FILE--
<?php

var_dump(jsond_decode('[""]'));

?>
--EXPECT--
array(1) {
  [0]=>
  string(0) ""
}
