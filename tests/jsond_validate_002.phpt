--TEST--
jsond_validate() - Error handling
--FILE--
<?php

require_once("jsond_validate_requires.inc");

jsond_validate_trycatchdump("");
jsond_validate_trycatchdump("-");
jsond_validate_trycatchdump("", -1);
jsond_validate_trycatchdump('{"key1":"value1", "key2":"value2"}', 1);
jsond_validate_trycatchdump('{"key1":"value1", "key2":"value2"}', 2);
jsond_validate_trycatchdump("-", 0);
jsond_validate_trycatchdump("-", 512, JSOND_BIGINT_AS_STRING);
jsond_validate_trycatchdump("-", 512, JSOND_BIGINT_AS_STRING | JSOND_INVALID_UTF8_IGNORE);
jsond_validate_trycatchdump("-", 512, JSOND_INVALID_UTF8_IGNORE);
jsond_validate_trycatchdump("{}", 512, JSOND_INVALID_UTF8_IGNORE);

?>
--EXPECTF--
bool(false)
int(4)
string(12) "Syntax error"
bool(false)
int(4)
string(12) "Syntax error"
bool(false)
int(4)
string(12) "Syntax error"
bool(false)
int(1)
string(28) "Maximum stack depth exceeded"
bool(true)
int(0)
string(8) "No error"
Error: 0 jsond_validate(): Argument #2 ($depth) must be greater than 0
int(0)
string(8) "No error"
Error: 0 jsond_validate(): Argument #3 ($flags) must be a valid flag (allowed flags: JSOND_INVALID_UTF8_IGNORE)
int(0)
string(8) "No error"
Error: 0 jsond_validate(): Argument #3 ($flags) must be a valid flag (allowed flags: JSOND_INVALID_UTF8_IGNORE)
int(0)
string(8) "No error"
bool(false)
int(4)
string(12) "Syntax error"
bool(true)
int(0)
string(8) "No error"
