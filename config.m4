dnl $Id$
dnl config.m4 for extension jsond

PHP_ARG_ENABLE(jsond, whether to enable new JavaScript Object Serialization support,
[  --disable-jsond          Disable new JavaScript Object Serialization support], yes)

if test "$PHP_JSOND" != "no"; then
  AC_DEFINE([HAVE_JSOND],1 ,[whether to enable new JavaScript Object Serialization support])
  AC_HEADER_STDC

  PHP_NEW_EXTENSION(jsond, 
      jsond.c \
      jsond_parser.tab.c \
      jsond_scanner.c,
      $ext_shared)
  PHP_INSTALL_HEADERS([ext/jsond], [php_jsond.h])
  PHP_SUBST(JSOND_SHARED_LIBADD)
fi
