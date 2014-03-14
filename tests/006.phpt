--TEST--
jsond_encode() & extended encoding
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php

$a = array('<foo>',"'bar'",'"baz"','&blong&');

echo "Normal: ", jsond_encode($a), "\n";
echo "Tags: ",   jsond_encode($a,JSOND_HEX_TAG), "\n";
echo "Apos: ",   jsond_encode($a,JSOND_HEX_APOS), "\n";
echo "Quot: ",   jsond_encode($a,JSOND_HEX_QUOT), "\n";
echo "Amp: ",    jsond_encode($a,JSOND_HEX_AMP), "\n";
echo "All: ",    jsond_encode($a,JSOND_HEX_TAG|JSOND_HEX_APOS|JSOND_HEX_QUOT|JSOND_HEX_AMP), "\n";
?>
--EXPECT--
Normal: ["<foo>","'bar'","\"baz\"","&blong&"]
Tags: ["\u003Cfoo\u003E","'bar'","\"baz\"","&blong&"]
Apos: ["<foo>","\u0027bar\u0027","\"baz\"","&blong&"]
Quot: ["<foo>","'bar'","\u0022baz\u0022","&blong&"]
Amp: ["<foo>","'bar'","\"baz\"","\u0026blong\u0026"]
All: ["\u003Cfoo\u003E","\u0027bar\u0027","\u0022baz\u0022","\u0026blong\u0026"]
