--TEST--
jsond_validate() - Error handling for max depth
--EXTENSIONS--
jsond
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php

try {
  var_dump(jsond_validate("-", PHP_INT_MAX));
} catch (ValueError $error) {
  echo $error->getMessage() . PHP_EOL;
  var_dump(jsond_last_error(), jsond_last_error_msg());
}

?>
--EXPECTF--
jsond_validate(): Argument #2 ($depth) must be less than %d
int(0)
string(8) "No error"
