PHP_ARG_ENABLE(jsond,
  [whether to enable jsond support],
  [AS_HELP_STRING([--enalbe-jsond],
                  [Enable new JavaScript Object Serialization support])],
  yes)

AC_ARG_ENABLE(jsond-buffer-native,
  [AS_HELP_STRING([--enable-jsond-buffer-native],
                  [Enable new jsond native buffer])],
  [AC_DEFINE([PHP_JSOND_BUF_TYPE_NATIVE],1,
             [whether native buffer is enabled])])

AC_ARG_ENABLE(jsond-filegen,
  [AS_HELP_STRING([--enable-jsond-filegen],
                  [Enable jsond parser and scanner files generation])],
  [PHP_JSOND_FILEGEN=yes], [PHP_JSOND_FILEGEN=no])

if test "$PHP_JSOND" != "no"; then
  AC_DEFINE([HAVE_JSOND],1 ,[whether to enable jsond support])
  PHP_JSO_SOURCES=m4_normalize(["
    jso/io/jso_io.c
    jso/io/jso_io_file.c
    jso/io/jso_io_memory.c
    jso/io/jso_io_string.c
    jso/jso_array.c
    jso/jso_builder.c
    jso/jso_cli.c
    jso/jso_dg_dtoa.c
    jso/jso_encoder.c
    jso/jso_error.c
    jso/jso_ht.c
    jso/jso_number.c
    jso/jso_object.c
    jso/jso_parser.c
    jso/jso_parser.tab.c
    jso/jso_re.c
    jso/jso_scanner.c
    jso/jso_value.c
    jso/parser/jso_parser_hooks_decode.c
    jso/pointer/jso_pointer.c
    jso/pointer/jso_pointer_error.c
    jso/schema/jso_schema_array.c
    jso/schema/jso_schema.c
    jso/schema/jso_schema_data.c
    jso/schema/jso_schema_error.c
    jso/schema/jso_schema_keyword_array.c
    jso/schema/jso_schema_keyword.c
    jso/schema/jso_schema_keyword_freer.c
    jso/schema/jso_schema_keyword_object.c
    jso/schema/jso_schema_keyword_regexp.c
    jso/schema/jso_schema_keyword_scalar.c
    jso/schema/jso_schema_keyword_single.c
    jso/schema/jso_schema_keyword_types.c
    jso/schema/jso_schema_keyword_union.c
    jso/schema/jso_schema_reference.c
    jso/schema/jso_schema_uri.c
    jso/schema/jso_schema_validation_array.c
    jso/schema/jso_schema_validation.c
    jso/schema/jso_schema_validation_common.c
    jso/schema/jso_schema_validation_composition.c
    jso/schema/jso_schema_validation_error.c
    jso/schema/jso_schema_validation_object.c
    jso/schema/jso_schema_validation_result.c
    jso/schema/jso_schema_validation_scalar.c
    jso/schema/jso_schema_validation_stack.c
    jso/schema/jso_schema_validation_stream.c
    jso/schema/jso_schema_validation_string.c
    jso/schema/jso_schema_validation_value.c
    jso/schema/jso_schema_value_allocator.c
    jso/schema/jso_schema_value_freer.c
    jso/schema/jso_schema_value_init.c
    jso/schema/jso_schema_value_parser.c
    jso/schema/jso_schema_version.c
  "])
  PHP_JSO_CFLAGS="-I@ext_srcdir@/jso -I@ext_srcdir@/jso/io -I@ext_srcdir@/jso/parser -I@ext_srcdir@/jso/pointer -I@ext_srcdir@/jso/schema"

  PHP_NEW_EXTENSION([jsond], m4_normalize([
      jsond.c
      jsond_buffer.c
      jsond_dtoa.c
      jsond_encoder.c
      jsond_parser.tab.c
      jsond_scanner.c
      jso_virt.c
      $PHP_JSO_SOURCES
    ]),
    [$ext_shared],,
    [$PHP_JSO_CFLAGS])

  PHP_INSTALL_HEADERS([ext/jsond], m4_normalize([
    php_jsond.h
    php_jsond_parser.h
    php_jsond_scanner.h
  ]))

  if test "$PHP_JSOND_FILEGEN" != "no"; then
    PHP_PROG_RE2C()
    PHP_PROG_BISON()
    PHP_ADD_MAKEFILE_FRAGMENT()
  fi
  AC_CHECK_SIZEOF(long)
  PHP_SUBST(JSOND_SHARED_LIBADD)
fi
