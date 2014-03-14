--TEST--
JSON (http://www.crockford.com/JSON/JSOND_checker/test/pass2.jsond)
--SKIPIF--
<?php
  if (!extension_loaded('jsond')) die('skip: jsond extension not available');
?>
--FILE--
<?php
    
$test = '[[[[[[[[[[[[[[[[[[["Not too deep"]]]]]]]]]]]]]]]]]]]';
echo 'Testing: ' . $test . "\n";
echo "DECODE: AS OBJECT\n";
$obj = jsond_decode($test);
var_dump($obj);
echo "DECODE: AS ARRAY\n";
$arr = jsond_decode($test, true);
var_dump($arr);

echo "ENCODE: FROM OBJECT\n";
$obj_enc = jsond_encode($obj);
echo $obj_enc . "\n";
echo "ENCODE: FROM ARRAY\n";
$arr_enc = jsond_encode($arr);
echo $arr_enc . "\n";

echo "DECODE AGAIN: AS OBJECT\n";
$obj = jsond_decode($obj_enc);
var_dump($obj);
echo "DECODE AGAIN: AS ARRAY\n";
$arr = jsond_decode($arr_enc, true);
var_dump($arr);

?>
--EXPECT--
Testing: [[[[[[[[[[[[[[[[[[["Not too deep"]]]]]]]]]]]]]]]]]]]
DECODE: AS OBJECT
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            array(1) {
              [0]=>
              array(1) {
                [0]=>
                array(1) {
                  [0]=>
                  array(1) {
                    [0]=>
                    array(1) {
                      [0]=>
                      array(1) {
                        [0]=>
                        array(1) {
                          [0]=>
                          array(1) {
                            [0]=>
                            array(1) {
                              [0]=>
                              array(1) {
                                [0]=>
                                array(1) {
                                  [0]=>
                                  array(1) {
                                    [0]=>
                                    array(1) {
                                      [0]=>
                                      string(12) "Not too deep"
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
DECODE: AS ARRAY
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            array(1) {
              [0]=>
              array(1) {
                [0]=>
                array(1) {
                  [0]=>
                  array(1) {
                    [0]=>
                    array(1) {
                      [0]=>
                      array(1) {
                        [0]=>
                        array(1) {
                          [0]=>
                          array(1) {
                            [0]=>
                            array(1) {
                              [0]=>
                              array(1) {
                                [0]=>
                                array(1) {
                                  [0]=>
                                  array(1) {
                                    [0]=>
                                    array(1) {
                                      [0]=>
                                      string(12) "Not too deep"
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
ENCODE: FROM OBJECT
[[[[[[[[[[[[[[[[[[["Not too deep"]]]]]]]]]]]]]]]]]]]
ENCODE: FROM ARRAY
[[[[[[[[[[[[[[[[[[["Not too deep"]]]]]]]]]]]]]]]]]]]
DECODE AGAIN: AS OBJECT
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            array(1) {
              [0]=>
              array(1) {
                [0]=>
                array(1) {
                  [0]=>
                  array(1) {
                    [0]=>
                    array(1) {
                      [0]=>
                      array(1) {
                        [0]=>
                        array(1) {
                          [0]=>
                          array(1) {
                            [0]=>
                            array(1) {
                              [0]=>
                              array(1) {
                                [0]=>
                                array(1) {
                                  [0]=>
                                  array(1) {
                                    [0]=>
                                    array(1) {
                                      [0]=>
                                      string(12) "Not too deep"
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
DECODE AGAIN: AS ARRAY
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(1) {
        [0]=>
        array(1) {
          [0]=>
          array(1) {
            [0]=>
            array(1) {
              [0]=>
              array(1) {
                [0]=>
                array(1) {
                  [0]=>
                  array(1) {
                    [0]=>
                    array(1) {
                      [0]=>
                      array(1) {
                        [0]=>
                        array(1) {
                          [0]=>
                          array(1) {
                            [0]=>
                            array(1) {
                              [0]=>
                              array(1) {
                                [0]=>
                                array(1) {
                                  [0]=>
                                  array(1) {
                                    [0]=>
                                    array(1) {
                                      [0]=>
                                      string(12) "Not too deep"
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
