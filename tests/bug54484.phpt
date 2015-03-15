--TEST--
Bug #54484 (Empty string in jsond_decode doesn't reset jsond_last_error)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
jsond_decode('{"test":"test"}');
var_dump(jsond_last_error());

jsond_decode("");
var_dump(jsond_last_error());


jsond_decode("invalid jsond");
var_dump(jsond_last_error());


jsond_decode("\"\001 invalid json\"");
var_dump(jsond_last_error());


jsond_decode("");
var_dump(jsond_last_error());
?>
--EXPECT--
int(0)
int(4)
int(4)
int(3)
int(4)
