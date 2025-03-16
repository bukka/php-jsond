<?php

/** @return string|false */
function jsond_encode(mixed $value, int $options = 0, int $depth = 512) {}

/** @return mixed */
function jsond_decode(string $json, ?bool $assoc = null, int $depth = 512, int $options = 0) {}

function jsond_last_error(): int {}

function jsond_last_error_msg(): string {}

interface JsondSerializable
{
    public function jsonSerialize(): mixed;
}
