--TEST--
jsond_encode() with non-packed array that should be encoded as an array rather than object
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
$a = array(1, 2, 3, 'foo' => 'bar');
unset($a['foo']);

var_dump(jsond_encode($a));
echo "Done\n";
?>
--EXPECT--
string(7) "[1,2,3]"
Done
