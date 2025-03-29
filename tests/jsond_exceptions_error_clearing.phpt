--TEST--
JSOND_THROW_ON_ERROR: global error flag untouched
--SKIPIF--
<?php
if (!extension_loaded("jsond")) {
     die('skip jsond extension not available');
}
?>
--FILE--
<?php

var_dump(jsond_last_error());

// here we cause a different kind of error to the following errors, so that
// we can be sure the global error state looking unchanged isn't coincidence
jsond_decode("\xFF");

var_dump(jsond_last_error());

try {
    jsond_decode("", false, 512, JSOND_THROW_ON_ERROR);
} catch (JsondException $e) {
    echo "Caught JSOND exception: ", $e->getCode(), PHP_EOL;
}

var_dump(jsond_last_error());

try {
    jsond_decode("{", false, 512, JSOND_THROW_ON_ERROR);
} catch (JsondException $e) {
    echo "Caught JSOND exception: ", $e->getCode(), PHP_EOL;
}

var_dump(jsond_last_error());


try {
    jsond_encode(NAN, JSOND_THROW_ON_ERROR);
} catch (JsondException $e) {
    echo "Caught JSOND exception: ", $e->getCode(), PHP_EOL;
}

var_dump(jsond_last_error());

?>
--EXPECT--
int(0)
int(5)
Caught JSOND exception: 4
int(5)
Caught JSOND exception: 4
int(5)
Caught JSOND exception: 7
int(5)
