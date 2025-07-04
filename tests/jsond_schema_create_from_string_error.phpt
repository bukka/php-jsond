--TEST--
JsondSchema::createFromString() - error
--EXTENSIONS--
jsond
--FILE--
<?php

$invalidSchemaJson = '{"type": "invalid_type"}';
try {
    $invalidSchema = JsondSchema::createFromString($invalidSchemaJson);
    echo "No exception thrown\n";
} catch (JsondSchemaException $e) {
    echo "JsonException caught: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
JsonException caught: JSON Schema parsing error (Invalid schema type invalid_type)
