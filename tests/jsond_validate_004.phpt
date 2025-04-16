--TEST--
jsond_validate() - Invalid UTF-8's
--EXTENSIONS--
jsond
--FILE--
<?php

require_once("jsond_validate_requires.inc");

echo "Testing Invalid UTF-8" . PHP_EOL;


jsond_validate_trycatchdump("\"a\xb0b\"");
jsond_validate_trycatchdump("\"a\xd0\xf2b\"");
jsond_validate_trycatchdump("\"\x61\xf0\x80\x80\x41\"");
jsond_validate_trycatchdump("[\"\xc1\xc1\",\"a\"]");

jsond_validate_trycatchdump("\"a\xb0b\"", 512, JSOND_INVALID_UTF8_IGNORE);
jsond_validate_trycatchdump("\"a\xd0\xf2b\"", 512, JSOND_INVALID_UTF8_IGNORE);
jsond_validate_trycatchdump("\"\x61\xf0\x80\x80\x41\"", 512, JSOND_INVALID_UTF8_IGNORE);
jsond_validate_trycatchdump("[\"\xc1\xc1\",\"a\"]", 512, JSOND_INVALID_UTF8_IGNORE);

?>
--EXPECT--
Testing Invalid UTF-8
bool(false)
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
bool(false)
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
bool(false)
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
bool(false)
int(5)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
bool(true)
int(0)
string(8) "No error"
bool(true)
int(0)
string(8) "No error"
bool(true)
int(0)
string(8) "No error"
bool(true)
int(0)
string(8) "No error"
