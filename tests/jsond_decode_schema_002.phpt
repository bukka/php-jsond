--TEST--
jsond_decode() - Schema validation 002 (allOf and exception handling)
--EXTENSIONS--
jsond
--FILE--
<?php

// Define a more complex JSON schema using allOf
$schemaJson = '{
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [
        {
            "type": "object",
            "properties": {
                "firstName": {
                    "type": "string",
                    "minLength": 2
                },
                "lastName": {
                    "type": "string",
                    "minLength": 2
                }
            },
            "required": ["firstName", "lastName"]
        },
        {
            "type": "object",
            "properties": {
                "age": {
                    "type": "integer",
                    "minimum": 18,
                    "maximum": 120
                },
                "role": {
                    "type": "string",
                    "enum": ["admin", "user", "guest"]
                }
            },
            "required": ["age"]
        }
    ]
}';

// Create schema object
$schema = JsondSchema::createFromString($schemaJson);

// Test valid JSON that satisfies both allOf schemas
$validJson = '{"firstName": "John", "lastName": "Doe", "age": 25, "role": "user"}';
var_dump(jsond_decode($validJson, true, 512, JSON_THROW_ON_ERROR, $schema));

try {
    // Test invalid JSON - fails first allOf schema (missing lastName)
    $invalidJson1 = '{"firstName": "Jane", "age": 30}';
    var_dump(jsond_decode($invalidJson1, true, 512, JSON_THROW_ON_ERROR, $schema));
} catch (JsondSchemaException $e) {
    echo "JsondSchemaException caught: " . $e->getMessage() . "\n";
    var_dump($e->getCode() === JSOND_ERROR_SCHEMA_VALIDATION_KEYWORD);
}

try {
// Test invalid JSON - fails second allOf schema (age below minimum)
    $invalidJson2 = '{"firstName": "Bob", "lastName": "Smith", "age": 16}';
    var_dump(jsond_decode($invalidJson2, true, 512, JSON_THROW_ON_ERROR, $schema));
} catch (JsondSchemaException $e) {
    echo "JsondSchemaException caught: " . $e->getMessage() . "\n";
    var_dump($e->getCode() === JSOND_ERROR_SCHEMA_VALIDATION_KEYWORD);
}

try {
    // Test invalid JSON - fails enum constraint
    $invalidJson3 = '{"firstName": "Alice", "lastName": "Johnson", "age": 28, "role": "moderator"}';
    var_dump(jsond_decode($invalidJson3, false, 512, JSON_THROW_ON_ERROR, $schema));
} catch (JsondSchemaException $e) {
    echo "JsondSchemaException caught: " . $e->getMessage() . "\n";
    var_dump($e->getCode() === JSOND_ERROR_SCHEMA_VALIDATION_KEYWORD);
}

// Test valid JSON with minimum requirements only
$validJson2 = '{"firstName": "Charlie", "lastName": "Brown", "age": 35}';
var_dump(jsond_decode($validJson2, false, 512, JSON_THROW_ON_ERROR, $schema));


?>
--EXPECT--
array(4) {
  ["firstName"]=>
  string(4) "John"
  ["lastName"]=>
  string(3) "Doe"
  ["age"]=>
  int(25)
  ["role"]=>
  string(4) "user"
}
JsondSchemaException caught: Object does not have required property with key lastName
bool(true)
JsondSchemaException caught: Value 16 is lower than minimum value 18
bool(true)
JsondSchemaException caught: Instance value not found in enum values
bool(true)
object(stdClass)#3 (3) {
  ["firstName"]=>
  string(7) "Charlie"
  ["lastName"]=>
  string(5) "Brown"
  ["age"]=>
  int(35)
}
