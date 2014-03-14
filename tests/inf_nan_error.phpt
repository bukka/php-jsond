--TEST--
An error is thrown when INF or NaN are encoded
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php

$inf = INF;

var_dump($inf);

var_dump(jsond_encode($inf));
var_dump(jsond_last_error(), jsond_last_error_msg());

var_dump(jsond_encode($inf, JSOND_PARTIAL_OUTPUT_ON_ERROR));
var_dump(jsond_last_error(), jsond_last_error_msg());

echo "\n";

$nan = NAN;

var_dump($nan);

var_dump(jsond_encode($nan));
var_dump(jsond_last_error(), jsond_last_error_msg());

var_dump(jsond_encode($nan, JSOND_PARTIAL_OUTPUT_ON_ERROR));
var_dump(jsond_last_error(), jsond_last_error_msg());
?>
--EXPECTF--
float(INF)
bool(false)
int(7)
string(34) "Inf and NaN cannot be JSON encoded"
string(1) "0"
int(7)
string(34) "Inf and NaN cannot be JSON encoded"

float(NAN)
bool(false)
int(7)
string(34) "Inf and NaN cannot be JSON encoded"
string(1) "0"
int(7)
string(34) "Inf and NaN cannot be JSON encoded"
