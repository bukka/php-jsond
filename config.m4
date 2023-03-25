dnl $Id$
dnl config.m4 for extension jsond

PHP_ARG_ENABLE(jsond,
  [whether to enable jsond support],
  [AS_HELP_STRING([--disable-jsond],
                  [Disable new JavaScript Object Serialization support])],
  yes)

AC_ARG_ENABLE(jsond-prefixing,
  [AS_HELP_STRING([--enable-jsond-with-json-prefix],
                  [Enable json prefixing])],
  [AC_DEFINE([PHP_JSOND_WITH_JSON_PREFIX],1,
             [whether json prefixing is enabled])])

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
  AC_HEADER_STDC

  PHP_NEW_EXTENSION(jsond,
      jsond.c \
      jsond_buffer.c \
      jsond_dtoa.c \
      jsond_encoder.c \
      jsond_parser.tab.c \
      jsond_scanner.c,
      $ext_shared)

  if test "$PHP_JSOND_FILEGEN" != "no"; then
    PHP_PROG_RE2C()
    PHP_PROG_BISON()
    PHP_ADD_MAKEFILE_FRAGMENT()
  fi
  PHP_INSTALL_HEADERS([ext/jsond], [php_jsond.h php_jsond_parser.h php_jsond_scanner.h])
  PHP_SUBST(JSOND_SHARED_LIBADD)
fi
