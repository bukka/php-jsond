--TEST--
Bug #41067 (jsond_encode() problem with UTF-16 input)
--SKIPIF--
<?php if (!extension_loaded("jsond")) print "skip"; ?>
--FILE--
<?php
$single_barline = "\360\235\204\200";
$array = array($single_barline);
print bin2hex($single_barline) . "\n";
// print $single_barline . "\n\n";
$jsond = jsond_encode($array);
print $jsond . "\n\n";
$jsond_decoded = jsond_decode($jsond, true);
// print $jsond_decoded[0] . "\n";
print bin2hex($jsond_decoded[0]) . "\n";
print "END\n";
?>
--EXPECT--
f09d8480
["\ud834\udd00"]

f09d8480
END
