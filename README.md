# PHP JSON extension with new Decoder

The php-jsond is a new JSON parser for PHP


## Installation

### Linux

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

## Benchmarks

The results are generated from [bench/compare_with_json.php](https://github.com/bukka/php-jsond/blob/master/bench/compare_with_json.php) with GCC (-O2) on x86_64.

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
