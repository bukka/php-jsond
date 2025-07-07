--TEST--
jsond_validate() - Schema validation 001 - simple object validation and last errors
--EXTENSIONS--
jsond
--FILE--
<?php

// Define a simple JSON schema for a person object
$schemaJson = '{
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
        "name": {
            "type": "string"
        },
        "age": {
            "type": "integer",
            "minimum": 0
        },
        "email": {
            "type": "string",
            "format": "email"
        }
    },
    "required": ["name", "age"]
}';

// Create schema object
$schema = JsondSchema::createFromString($schemaJson);

// Test valid JSON
$validJson = '{"name": "John Doe", "age": 30, "email": "john@example.com"}';
var_dump(jsond_validate($validJson, 512, 0, $schema));

// Test invalid JSON - missing required field
$invalidJson1 = '{"name": "Jane Doe"}';
var_dump(jsond_validate($invalidJson1, 512, 0, $schema));
var_dump(jsond_last_error_msg());
var_dump(jsond_last_error() == JSOND_ERROR_SCHEMA_VALIDATION_KEYWORD);

// Test invalid JSON - wrong type
$invalidJson2 = '{"name": "Bob", "age": "thirty"}';
var_dump(jsond_validate($invalidJson2, 512, 0, $schema));
var_dump(jsond_last_error_msg());
var_dump(jsond_last_error() == JSOND_ERROR_SCHEMA_VALIDATION_TYPE);

// Test invalid JSON - negative age
$invalidJson3 = '{"name": "Alice", "age": -5}';
var_dump(jsond_validate($invalidJson3, 512, 0, $schema));
var_dump(jsond_last_error_msg());
var_dump(jsond_last_error() == JSOND_ERROR_SCHEMA_VALIDATION_KEYWORD);

// Test without schema (should always return true for valid JSON)
$validJsonNoSchema = '{"any": "data", "works": true}';
var_dump(jsond_validate($validJsonNoSchema));
var_dump(jsond_last_error_msg());
var_dump(jsond_last_error());

?>
--EXPECT--
bool(true)
bool(false)
string(84) "JSON schema validation keyword (Object does not have required property with key age)"
bool(true)
bool(false)
string(97) "JSON schema validation type (Invalid validation type, expected int or double but received string)"
bool(true)
bool(false)
string(71) "JSON schema validation keyword (Value -5 is lower than minimum value 0)"
bool(true)
bool(true)
string(8) "No error"
int(0)
