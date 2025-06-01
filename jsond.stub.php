<?php

/** @generate-class-entries */

interface JsondSerializable
{
    public function jsonSerialize(): mixed;
}

class JsondException extends Exception
{
}

class JsondSchemaException extends JsondException
{
}

/** @not-serializable */
class JsondSchema
{
    private final function __construct() {}

    public static function createFromString(string $source): JsondSchema {}
}

function jsond_encode(mixed $value, int $flags = 0, int $depth = 512): string|false {}

function jsond_decode(string $json, ?bool $associative = null, int $depth = 512, int $flags = 0, ?JsondSchema $schema = null): mixed {}

function jsond_validate(string $json, int $depth = 512, int $flags = 0, ?JsondSchema $schema = null): bool {}

function jsond_last_error(): int {}

function jsond_last_error_msg(): string {}
