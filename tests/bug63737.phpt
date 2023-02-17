--TEST--
Bug #63737 (json_decode does not properly decode with options parameter)
--INI--
serialize_precision=14
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

function decode($json) {
    global $jsond_decode;

    $x = $jsond_decode($json);
    var_dump($x);
    $x = $jsond_decode($json, false, 512, jsond_constant('BIGINT_AS_STRING'));
    var_dump($x);
}

decode('123456789012345678901234567890');
decode('-123456789012345678901234567890');

// This shouldn't affect floats, but let's check that.
decode('123456789012345678901234567890.1');
decode('-123456789012345678901234567890.1');

echo "Done\n";
?>
--EXPECT--
float(1.2345678901235E+29)
string(30) "123456789012345678901234567890"
float(-1.2345678901235E+29)
string(31) "-123456789012345678901234567890"
float(1.2345678901235E+29)
float(1.2345678901235E+29)
float(-1.2345678901235E+29)
float(-1.2345678901235E+29)
Done
