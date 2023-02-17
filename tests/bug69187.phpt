--TEST--
Bug #69187 json_last_error return BC in PHP7
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

var_dump($jsond_decode("NULL"));
var_dump($jsond_last_error());
var_dump($jsond_decode(FALSE));
var_dump($jsond_last_error());
var_dump($jsond_decode(""));
var_dump($jsond_last_error());
var_dump($jsond_decode(0));
var_dump($jsond_last_error());
var_dump($jsond_decode(1));
var_dump($jsond_last_error());
var_dump($jsond_decode(TRUE));
var_dump($jsond_last_error());

$jsond_decode("\xED\xA0\xB4");
var_dump($jsond_last_error());

$jsond_decode("\x00");
var_dump($jsond_last_error());

$jsond_decode("\"\xED\xA0\xB4\"");
var_dump($jsond_last_error());

$jsond_decode("\"\x00\"");
var_dump($jsond_last_error());
?>
--EXPECT--
NULL
int(4)
NULL
int(4)
NULL
int(4)
int(0)
int(0)
int(1)
int(0)
int(1)
int(0)
int(5)
int(3)
int(5)
int(3)
