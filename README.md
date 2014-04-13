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

First download the source
```
wget -O - https://github.com/bukka/php-jsond/archive/master.tar.gz | tar xzp
```
or if you prefer to clone the repository
```
git clone https://github.com/bukka/php-jsond.git
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

Constants have different prefix (`JSOND`).
```
JSOND_ERROR_NONE (integer)
JSOND_ERROR_DEPTH (integer)
JSOND_ERROR_STATE_MISMATCH (integer)
JSOND_ERROR_CTRL_CHAR (integer)
JSOND_ERROR_SYNTAX (integer)
JSOND_ERROR_UTF8 (integer)
JSOND_ERROR_RECURSION (integer)
JSOND_ERROR_INF_OR_NAN (integer)
JSOND_ERROR_UNSUPPORTED_TYPE (integer
JSOND_ERROR_UTF16 (integer)
```
The error codes are the same as json errors except `JSOND_ERROR_UTF16` that is new. This error is raised if escaped unicode is an invalid surrogate pair (lead surrogate is not fallowed by trail surrogate).

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
```
The constants are the same as json constants.

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

If PHP is compiled without json extension (`--without-json`) and jsond is compiled with defined macro `PHP_JSOND_PRIMARY` (non-default), the API is exactly the same as the API documented in [JSON documentation](http://php.net/json). There is just one small difference that a new error constant is defined - `JSON_ERROR_UTF16` that is described above.


## Benchmarks

The results are generated from [bench/compare_with_json.php](https://github.com/bukka/php-jsond/blob/master/bench/compare_with_json.php) compiled with GCC (-O2) on x86_64.

```
STR: {"i": 23, "array": [1, null, false, true, ["aha", "baba", 23, {"test": 23}]]}
JSON:  time for 100000 iterations: 0.238321
JSOND: time for 100000 iterations: 0.236436
STR: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
JSON:  time for 100000 iterations: 0.110764
JSOND: time for 100000 iterations: 0.134212
STR: {"a": 23.2234232}
JSON:  time for 100000 iterations: 0.078458
JSOND: time for 100000 iterations: 0.055479
STR: {"long-str": "At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et expedita distinctio. Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellendus. Temporibus autem quibusdam et aut officiis debitis aut rerum necessitatibus saepe eveniet ut et voluptates repudiandae sint et molestiae non recusandae. Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis voluptatibus maiores alias consequatur aut perferendis doloribus asperiores repellat.", }
JSON:  time for 100000 iterations: 0.881429
JSOND: time for 100000 iterations: 0.118529
STR: ... json_encode($_SERVER)
JSON:  time for 100000 iterations: 4.341252
JSOND: time for 100000 iterations: 1.960814
```

As you can see the new parser (JSOND) is much faster for long strings.


## Upgrading from php-json

There are some small incompatibilities with json ext. They are documented in [UPGRADE.md](https://github.com/bukka/php-jsond/blob/master/TODO.md)

## TODO list

The TODO list can be found in [TODO.md](https://github.com/bukka/php-jsond/blob/master/TODO.md).
