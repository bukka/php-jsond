--TEST--
jsond_decode() tests
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php

var_dump(jsond_decode());
var_dump(jsond_decode(""));
var_dump(jsond_decode("", 1));
var_dump(jsond_decode("", 0));
var_dump(jsond_decode(".", 1));
var_dump(jsond_decode(".", 0));
var_dump(jsond_decode("<?>"));
var_dump(jsond_decode(";"));
var_dump(jsond_decode("руссиш"));
var_dump(jsond_decode("blah"));
var_dump(jsond_decode(NULL));
var_dump(jsond_decode('{ "test": { "foo": "bar" } }'));
var_dump(jsond_decode('{ "test": { "foo": "" } }'));
var_dump(jsond_decode('{ "": { "foo": "" } }'));
var_dump(jsond_decode('{ "": { "": "" } }'));
var_dump(jsond_decode('{ "": { "": "" }'));
var_dump(jsond_decode('{ "": "": "" } }'));

?>
===DONE===
--EXPECTF--
Warning: jsond_decode() expects at least 1 parameter, 0 given in %s on line %d
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
object(stdClass)#%d (1) {
  ["test"]=>
  object(stdClass)#%d (1) {
    ["foo"]=>
    string(3) "bar"
  }
}
object(stdClass)#%d (1) {
  ["test"]=>
  object(stdClass)#%d (1) {
    ["foo"]=>
    string(0) ""
  }
}
object(stdClass)#%d (1) {
  ["_empty_"]=>
  object(stdClass)#%d (1) {
    ["foo"]=>
    string(0) ""
  }
}
object(stdClass)#%d (1) {
  ["_empty_"]=>
  object(stdClass)#%d (1) {
    ["_empty_"]=>
    string(0) ""
  }
}
NULL
NULL
===DONE===
