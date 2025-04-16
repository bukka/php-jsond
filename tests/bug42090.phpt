--TEST--
Bug #42090 (jsond_decode causes segmentation fault)
--EXTENSIONS--
jsond
--FILE--
<?php
require_once "bootstrap.inc";

var_dump(
    jsond_decode('""'),
    jsond_decode('"..".'),
    jsond_decode('"'),
    jsond_decode('""""'),
    jsond_encode('"'),
    jsond_decode(jsond_encode('"')),
    jsond_decode(jsond_encode('""'))
);
?>
--EXPECT--
string(0) ""
NULL
NULL
NULL
string(4) ""\"""
string(1) """
string(2) """"
