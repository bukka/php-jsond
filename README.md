# PHP JSON extension with new Decoder

The php-jsond is a new JSON parser for PHP


## Installation

### Linux

#### PECL

This extension is available on PECL. The package is not currently stable. If the config `preferre_state` is stable, then the version needs to be specified.

```
$ sudo pecl install jsond-1.x.y
```

where `x` is an installed minor version number and `y` bug fixing version number.


#### Manual Installation

It's important to have a git installed as it's necessary for recursive fetch of
[phpc](https://github.com/bukka/phpc).

First clone recursively the repository
```
git clone --recursive https://github.com/bukka/php-jsond.git
```

Then go to the created directory and compile the extension. The PHP development package has to be installed (command `phpize` must be available).
```
cd php-jsond
phpize
./configure
make
sudo make install
```

### Windows

Precompiled binary `dll` libraries for php-jsond are available on [the PECL jsond page](http://pecl.php.net/package/jsond).


## Documentation

### Default API

The functionality is exactly the same as documented in [JSON documentation](http://php.net/json), just the prefixes are different.


#### Constants

Constants have different prefix (`JSOND`) than JSON constants but they are the same as the one in PHP 7.

##### Error constants
```
JSOND_ERROR_NONE (integer)
JSOND_ERROR_DEPTH (integer)
JSOND_ERROR_STATE_MISMATCH (integer)
JSOND_ERROR_CTRL_CHAR (integer)
JSOND_ERROR_SYNTAX (integer)
JSOND_ERROR_UTF8 (integer)
JSOND_ERROR_RECURSION (integer)
JSOND_ERROR_INF_OR_NAN (integer)
JSOND_ERROR_UNSUPPORTED_TYPE (integer)
JSOND_ERROR_INVALID_PROPERTY_NAME (integer)
JSOND_ERROR_UTF16 (integer)
```

##### Encoding constants
```
JSOND_HEX_TAG (integer)
JSOND_HEX_AMP (integer)
JSOND_HEX_APOS (integer)
JSOND_HEX_QUOT (integer)
JSOND_FORCE_OBJECT (integer)
JSOND_NUMERIC_CHECK (integer)
JSOND_BIGINT_AS_STRING (integer)
JSOND_PRETTY_PRINT (integer)
JSOND_UNESCAPED_SLASHES (integer)
JSOND_UNESCAPED_UNICODE (integer)
JSOND_PARTIAL_OUTPUT_ON_ERROR (integer)
JSOND_PRESERVE_ZERO_FRACTION (integer)
JSOND_VALID_ESCAPE_UNICODE (integer)
```

##### Decoding constants
```
JSOND_OBJECT_AS_ARRAY (integer)
JSOND_BIGINT_AS_STRING (integer)
```

#### Functions

The prefix for functions is `jsond`.

```php
mixed jsond_decode ( string $json [, bool $assoc = false [, int $depth = 512 [, int $options = 0 ]]] );
string jsond_encode ( mixed $value [, int $options = 0 [, int $depth = 512 ]] );
string jsond_last_error_msg ( void );
int jsond_last_error ( void );
```

####  The JsondSerializable interface

Prefix for interface is `Jsond` otherwise it is the same as `JsonSerializable`. 

```php
JsondSerializable {
    /* Methods */
    abstract public mixed jsonSerialize ( void )
}
```


### Drop-in alternative for the standard JSON extension

If jsond is compiled with `--enable-jsond-with-json-prefix`, than the json functions are replaced
with jsond variants and the API is exactly the same as the API documented in [JSON documentation](http://php.net/json).

### JSOND IDE Auto Complete
If you use IDE like PhpStorm, you can set the file `utils/jsond_auto_complete.php` to the Include Path,
then IDE will auto complete `jsond_encode` and `jsond_decode`.

## Upgrading from php-json

All changes are listed in [UPGRADE.md](https://github.com/bukka/php-jsond/blob/master/UPGRADE.md)

## TODO list

The TODO list can be found in [TODO.md](https://github.com/bukka/php-jsond/blob/master/TODO.md).
