--TEST--
jsond_encode() & extended encoding
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
require_once "bootstrap.inc";

$a = array('<foo>',"'bar'",'"baz"','&blong&');

echo "Normal: ", $jsond_encode($a), "\n";
echo "Tags: ",   $jsond_encode($a, jsond_constant('HEX_TAG')), "\n";
echo "Apos: ",   $jsond_encode($a, jsond_constant('HEX_APOS')), "\n";
echo "Quot: ",   $jsond_encode($a, jsond_constant('HEX_QUOT')), "\n";
echo "Amp: ",    $jsond_encode($a, jsond_constant('HEX_AMP')), "\n";
echo "All: ",    $jsond_encode($a, jsond_constant('HEX_TAG', 'HEX_APOS', 'HEX_QUOT', 'HEX_AMP')), "\n";
?>
--EXPECT--
Normal: ["<foo>","'bar'","\"baz\"","&blong&"]
Tags: ["\u003Cfoo\u003E","'bar'","\"baz\"","&blong&"]
Apos: ["<foo>","\u0027bar\u0027","\"baz\"","&blong&"]
Quot: ["<foo>","'bar'","\u0022baz\u0022","&blong&"]
Amp: ["<foo>","'bar'","\"baz\"","\u0026blong\u0026"]
All: ["\u003Cfoo\u003E","\u0027bar\u0027","\u0022baz\u0022","\u0026blong\u0026"]
