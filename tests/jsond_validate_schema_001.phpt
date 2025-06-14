--TEST--
jsond_validate() - General usage
--EXTENSIONS--
jsond
--FILE--
<?php

var_dump(
  jsond_validate(""),
  jsond_validate("."),
  jsond_validate("<?>"),
  jsond_validate(";"),
  jsond_validate("руссиш"),
  jsond_validate("blah"),
  jsond_validate('{ "": "": "" } }'),
  jsond_validate('{ "": { "": "" }'),
  jsond_validate('{ "test": {} "foo": "bar" }, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'),

  jsond_validate('{ "test": { "foo": "bar" } }'),
  jsond_validate('{ "test": { "foo": "" } }'),
  jsond_validate('{ "": { "foo": "" } }'),
  jsond_validate('{ "": { "": "" } }'),
  jsond_validate('{ "test": {"foo": "bar"}, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'),
  jsond_validate('{ "test": {"foo": "bar"}, "test2": {"foo" : "bar" }, "test3": {"foo" : "bar" } }'),
);

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
