--TEST--
json_encode() with JSON_PRETTY_PRINT
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

function encode_decode($jsond) {
    global $jsond_encode, $jsond_decode;

	$struct = $jsond_decode($jsond);
	$pretty = $jsond_encode($struct, jsond_constant('PRETTY_PRINT'));
	echo "$pretty\n";
	$pretty = $jsond_decode($pretty);
	printf("Match: %d\n", $pretty == $struct);
}

encode_decode('[1,2,3,[1,2,3]]');
encode_decode('{"a":1,"b":[1,2],"c":{"d":42}}');
?>
--EXPECT--
[
    1,
    2,
    3,
    [
        1,
        2,
        3
    ]
]
Match: 1
{
    "a": 1,
    "b": [
        1,
        2
    ],
    "c": {
        "d": 42
    }
}
Match: 1
