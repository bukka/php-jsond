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