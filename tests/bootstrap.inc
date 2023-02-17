<?php
global $jsond_prefix, $jsond_encode, $jsond_decode,
    $jsond_last_error, $jsond_last_error_msg;

$jsond_prefix = function_exists('jsond_encode') ? 'jsond' : 'json';
$jsond_encode = $jsond_prefix . '_encode';
$jsond_decode = $jsond_prefix . '_decode';
$jsond_last_error = $jsond_prefix . '_last_error';
$jsond_last_error_msg = $jsond_prefix . '_last_error_msg';

function jsond_constant() {
    global $jsond_prefix;

    $constant_value = 0;
    $constants = func_get_args();
    foreach ($constants as $constant) {
        $constant_value |= constant(strtoupper($jsond_prefix) . '_' . $constant);
    }

    return $constant_value;
}

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
