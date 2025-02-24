<?php

function jsond_expect_argument_count_error($wrapper, $expected_message_prefix, $warning) {
    try {
        var_dump($wrapper());
    } catch (ArgumentCountError $exception) {
        if (strpos($exception->getMessage(), $expected_message_prefix) === false) {
            echo "ERROR: " . $exception->getMessage() . "\n";
        } else {
            echo "\n" . $warning . "\n";
            var_dump(NULL);
        }
    }
}
