/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_PHP_JSOND_YY_JSOND_PARSER_TAB_H_INCLUDED
# define YY_PHP_JSOND_YY_JSOND_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef PHP_JSOND_YYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define PHP_JSOND_YYDEBUG 1
#  else
#   define PHP_JSOND_YYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define PHP_JSOND_YYDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined PHP_JSOND_YYDEBUG */
#if PHP_JSOND_YYDEBUG
extern int php_jsond_yydebug;
#endif

/* Token kinds.  */
#ifndef PHP_JSOND_YYTOKENTYPE
# define PHP_JSOND_YYTOKENTYPE
  enum php_jsond_yytokentype
  {
    PHP_JSOND_YYEMPTY = -2,
    PHP_JSOND_YYEOF = 0,           /* "end of file"  */
    PHP_JSOND_YYerror = 256,       /* error  */
    PHP_JSOND_YYUNDEF = 257,       /* "invalid token"  */
    PHP_JSOND_T_NUL = 258,         /* PHP_JSOND_T_NUL  */
    PHP_JSOND_T_TRUE = 259,        /* PHP_JSOND_T_TRUE  */
    PHP_JSOND_T_FALSE = 260,       /* PHP_JSOND_T_FALSE  */
    PHP_JSOND_T_INT = 261,         /* PHP_JSOND_T_INT  */
    PHP_JSOND_T_DOUBLE = 262,      /* PHP_JSOND_T_DOUBLE  */
    PHP_JSOND_T_STRING = 263,      /* PHP_JSOND_T_STRING  */
    PHP_JSOND_T_ESTRING = 264,     /* PHP_JSOND_T_ESTRING  */
    PHP_JSOND_T_EOI = 265,         /* PHP_JSOND_T_EOI  */
    PHP_JSOND_T_ERROR = 266        /* PHP_JSOND_T_ERROR  */
  };
  typedef enum php_jsond_yytokentype php_jsond_yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined PHP_JSOND_YYSTYPE && ! defined PHP_JSOND_YYSTYPE_IS_DECLARED
union PHP_JSOND_YYSTYPE
{

	zval value;


};
typedef union PHP_JSOND_YYSTYPE PHP_JSOND_YYSTYPE;
# define PHP_JSOND_YYSTYPE_IS_TRIVIAL 1
# define PHP_JSOND_YYSTYPE_IS_DECLARED 1
#endif




int php_jsond_yyparse (php_jsond_parser *parser);


#endif /* !YY_PHP_JSOND_YY_JSOND_PARSER_TAB_H_INCLUDED  */
