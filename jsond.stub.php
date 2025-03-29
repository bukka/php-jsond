<?php

/** @generate-function-entries */

/** @return string|false */
function jsond_encode(mixed $value, int $flags = 0, int $depth = 512): string|false {}

/** @return mixed */
function jsond_decode(string $json, ?bool $associative = null, int $depth = 512, int $flags = 0) {}

function jsond_last_error(): int {}

function jsond_last_error_msg(): string {}

interface JsondSerializable
{
    public function jsonSerialize(): mixed;
}
