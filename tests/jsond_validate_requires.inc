<?php

function jsond_validate_trycatchdump($json, $depth = 512, $flags = 0) {
    try {
        var_dump(jsond_validate($json, $depth, $flags));
    } catch (JsonException $e) {
        echo "JsonException: {$e->getCode()} {$e->getMessage()}". PHP_EOL;
    } catch (Exception $e) {
        echo "Exception: {$e->getCode()} {$e->getMessage()}". PHP_EOL;
    } catch (Error $e) {
        echo "Error: {$e->getCode()} {$e->getMessage()}". PHP_EOL;
    }

    var_dump(jsond_last_error(), jsond_last_error_msg());
}

?>
