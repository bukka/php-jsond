--TEST--
Test jsond_encode() function : JSOND_THROW_ON_ERROR flag
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
     die('skip jsond extension not available');
}
?>
--FILE--
<?php

try {
    var_dump(jsond_encode("\x80", JSOND_THROW_ON_ERROR));
} catch (JsondException $e) {
    var_dump($e);
}

// JSOND_PARTIAL_OUTPUT_ON_ERROR is incompatible with exceptions
// So it overrides it for the sake of working with wrappers that add the
// JSOND_THROW_ON_ERROR flag
var_dump(jsond_encode("\x80", JSOND_THROW_ON_ERROR | JSOND_PARTIAL_OUTPUT_ON_ERROR));
var_dump(jsond_last_error());
var_dump(jsond_last_error_msg());

?>
--EXPECTF--
object(JsondException)#1 (7) {
  ["message":protected]=>
  string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
  ["string":"Exception":private]=>
  string(0) ""
  ["code":protected]=>
  int(5)
  ["file":protected]=>
  string(%d) "%s"
  ["line":protected]=>
  int(%d)
  ["trace":"Exception":private]=>
  array(1) {
    [0]=>
    array(4) {
      ["file"]=>
      string(%d) "%s"
      ["line"]=>
      int(%d)
      ["function"]=>
      string(12) "jsond_encode"
      ["args"]=>
      array(2) {
        [0]=>
        string(1) "%s"
        [1]=>
        int(4194304)
      }
    }
  }
  ["previous":"Exception":private]=>
  NULL
}
string(4) "null"
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
