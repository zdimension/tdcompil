/* A Bison parser, made by GNU Bison 3.7.6.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30706

/* Bison version string.  */
#define YYBISON_VERSION "3.7.6"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 8 "syntax.y"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "calc.h"
#include "code.h"

// Yacc prototypes
int  yylex(void);
void yyerror(const char *s);


#define  YYERROR_VERBOSE 1      // produce verbose syntax error messages

#line 89 "syntax.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NUMBER = 258,                  /* NUMBER  */
    IDENT = 259,                   /* IDENT  */
    STRING = 260,                  /* STRING  */
    KWHILE = 261,                  /* KWHILE  */
    KIF = 262,                     /* KIF  */
    KPRINT = 263,                  /* KPRINT  */
    KELSE = 264,                   /* KELSE  */
    KREAD = 265,                   /* KREAD  */
    KFOR = 266,                    /* KFOR  */
    KDO = 267,                     /* KDO  */
    KVAR = 268,                    /* KVAR  */
    KFUNC = 269,                   /* KFUNC  */
    KRETURN = 270,                 /* KRETURN  */
    KBREAK = 271,                  /* KBREAK  */
    KCONTINUE = 272,               /* KCONTINUE  */
    KCONST = 273,                  /* KCONST  */
    KTYPE = 274,                   /* KTYPE  */
    KTYPEOF = 275,                 /* KTYPEOF  */
    KSIZEOF = 276,                 /* KSIZEOF  */
    KSTRUCT = 277,                 /* KSTRUCT  */
    KBITSOF = 278,                 /* KBITSOF  */
    KNEW = 279,                    /* KNEW  */
    KASSERT = 280,                 /* KASSERT  */
    KLOOP = 281,                   /* KLOOP  */
    KMATCH = 282,                  /* KMATCH  */
    KIS = 283,                     /* KIS  */
    RANGE = 284,                   /* RANGE  */
    IRANGE = 285,                  /* IRANGE  */
    KGLOBAL = 286,                 /* KGLOBAL  */
    KIMPL = 287,                   /* KIMPL  */
    KIN = 288,                     /* KIN  */
    KFOREACH = 289,                /* KFOREACH  */
    KINTERFACE = 290,              /* KINTERFACE  */
    KSELF = 291,                   /* KSELF  */
    GE = 292,                      /* GE  */
    LE = 293,                      /* LE  */
    EQ = 294,                      /* EQ  */
    NE = 295,                      /* NE  */
    REF = 296,                     /* REF  */
    DEREF = 297,                   /* DEREF  */
    APL = 298,                     /* APL  */
    AMN = 299,                     /* AMN  */
    AML = 300,                     /* AML  */
    ADV = 301,                     /* ADV  */
    INC = 302,                     /* INC  */
    DEC = 303,                     /* DEC  */
    AND = 304,                     /* AND  */
    OR = 305,                      /* OR  */
    SHL = 306,                     /* SHL  */
    SHR = 307,                     /* SHR  */
    ARROW = 308,                   /* ARROW  */
    STRUCTLIT = 309,               /* STRUCTLIT  */
    GENINST = 310,                 /* GENINST  */
    PIPELINE = 311,                /* PIPELINE  */
    CAST = 312,                    /* CAST  */
    UMINUS = 313,                  /* UMINUS  */
    VDECL = 314,                   /* VDECL  */
    SCOPE = 315,                   /* SCOPE  */
    TUPLEASSIGN = 316,             /* TUPLEASSIGN  */
    THEN = 317                     /* THEN  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 33 "syntax.y"

	struct { int value; int size; } number; // number value
    char *var;                   // ident name
    int chr;
    ast_node *node;              // node pointer

#line 205 "syntax.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);


/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUMBER = 3,                     /* NUMBER  */
  YYSYMBOL_IDENT = 4,                      /* IDENT  */
  YYSYMBOL_STRING = 5,                     /* STRING  */
  YYSYMBOL_KWHILE = 6,                     /* KWHILE  */
  YYSYMBOL_KIF = 7,                        /* KIF  */
  YYSYMBOL_KPRINT = 8,                     /* KPRINT  */
  YYSYMBOL_KELSE = 9,                      /* KELSE  */
  YYSYMBOL_KREAD = 10,                     /* KREAD  */
  YYSYMBOL_KFOR = 11,                      /* KFOR  */
  YYSYMBOL_KDO = 12,                       /* KDO  */
  YYSYMBOL_KVAR = 13,                      /* KVAR  */
  YYSYMBOL_KFUNC = 14,                     /* KFUNC  */
  YYSYMBOL_KRETURN = 15,                   /* KRETURN  */
  YYSYMBOL_KBREAK = 16,                    /* KBREAK  */
  YYSYMBOL_KCONTINUE = 17,                 /* KCONTINUE  */
  YYSYMBOL_KCONST = 18,                    /* KCONST  */
  YYSYMBOL_KTYPE = 19,                     /* KTYPE  */
  YYSYMBOL_KTYPEOF = 20,                   /* KTYPEOF  */
  YYSYMBOL_KSIZEOF = 21,                   /* KSIZEOF  */
  YYSYMBOL_KSTRUCT = 22,                   /* KSTRUCT  */
  YYSYMBOL_KBITSOF = 23,                   /* KBITSOF  */
  YYSYMBOL_KNEW = 24,                      /* KNEW  */
  YYSYMBOL_KASSERT = 25,                   /* KASSERT  */
  YYSYMBOL_KLOOP = 26,                     /* KLOOP  */
  YYSYMBOL_KMATCH = 27,                    /* KMATCH  */
  YYSYMBOL_KIS = 28,                       /* KIS  */
  YYSYMBOL_RANGE = 29,                     /* RANGE  */
  YYSYMBOL_IRANGE = 30,                    /* IRANGE  */
  YYSYMBOL_KGLOBAL = 31,                   /* KGLOBAL  */
  YYSYMBOL_KIMPL = 32,                     /* KIMPL  */
  YYSYMBOL_KIN = 33,                       /* KIN  */
  YYSYMBOL_KFOREACH = 34,                  /* KFOREACH  */
  YYSYMBOL_KINTERFACE = 35,                /* KINTERFACE  */
  YYSYMBOL_KSELF = 36,                     /* KSELF  */
  YYSYMBOL_37_ = 37,                       /* '+'  */
  YYSYMBOL_38_ = 38,                       /* '-'  */
  YYSYMBOL_39_ = 39,                       /* '*'  */
  YYSYMBOL_40_ = 40,                       /* '/'  */
  YYSYMBOL_GE = 41,                        /* GE  */
  YYSYMBOL_LE = 42,                        /* LE  */
  YYSYMBOL_EQ = 43,                        /* EQ  */
  YYSYMBOL_NE = 44,                        /* NE  */
  YYSYMBOL_45_ = 45,                       /* '>'  */
  YYSYMBOL_46_ = 46,                       /* '<'  */
  YYSYMBOL_REF = 47,                       /* REF  */
  YYSYMBOL_DEREF = 48,                     /* DEREF  */
  YYSYMBOL_APL = 49,                       /* APL  */
  YYSYMBOL_AMN = 50,                       /* AMN  */
  YYSYMBOL_AML = 51,                       /* AML  */
  YYSYMBOL_ADV = 52,                       /* ADV  */
  YYSYMBOL_INC = 53,                       /* INC  */
  YYSYMBOL_DEC = 54,                       /* DEC  */
  YYSYMBOL_AND = 55,                       /* AND  */
  YYSYMBOL_OR = 56,                        /* OR  */
  YYSYMBOL_SHL = 57,                       /* SHL  */
  YYSYMBOL_SHR = 58,                       /* SHR  */
  YYSYMBOL_ARROW = 59,                     /* ARROW  */
  YYSYMBOL_STRUCTLIT = 60,                 /* STRUCTLIT  */
  YYSYMBOL_GENINST = 61,                   /* GENINST  */
  YYSYMBOL_PIPELINE = 62,                  /* PIPELINE  */
  YYSYMBOL_CAST = 63,                      /* CAST  */
  YYSYMBOL_UMINUS = 64,                    /* UMINUS  */
  YYSYMBOL_VDECL = 65,                     /* VDECL  */
  YYSYMBOL_SCOPE = 66,                     /* SCOPE  */
  YYSYMBOL_TUPLEASSIGN = 67,               /* TUPLEASSIGN  */
  YYSYMBOL_THEN = 68,                      /* THEN  */
  YYSYMBOL_69_ = 69,                       /* ';'  */
  YYSYMBOL_70_ = 70,                       /* '('  */
  YYSYMBOL_71_ = 71,                       /* ')'  */
  YYSYMBOL_72_ = 72,                       /* '='  */
  YYSYMBOL_73_ = 73,                       /* '{'  */
  YYSYMBOL_74_ = 74,                       /* '}'  */
  YYSYMBOL_75_ = 75,                       /* ':'  */
  YYSYMBOL_76_ = 76,                       /* ','  */
  YYSYMBOL_77_ = 77,                       /* '['  */
  YYSYMBOL_78_ = 78,                       /* ']'  */
  YYSYMBOL_79___ = 79,                     /* '_'  */
  YYSYMBOL_80_ = 80,                       /* '|'  */
  YYSYMBOL_81_ = 81,                       /* '.'  */
  YYSYMBOL_82_ = 82,                       /* '~'  */
  YYSYMBOL_83_ = 83,                       /* '&'  */
  YYSYMBOL_YYACCEPT = 84,                  /* $accept  */
  YYSYMBOL_session = 85,                   /* session  */
  YYSYMBOL_stmts = 86,                     /* stmts  */
  YYSYMBOL_stmt = 87,                      /* stmt  */
  YYSYMBOL_func_signature = 88,            /* func_signature  */
  YYSYMBOL_func = 89,                      /* func  */
  YYSYMBOL_func_list = 90,                 /* func_list  */
  YYSYMBOL_tuple_assign_left = 91,         /* tuple_assign_left  */
  YYSYMBOL_tuple_assign_right = 92,        /* tuple_assign_right  */
  YYSYMBOL_expr_or_inline_decl = 93,       /* expr_or_inline_decl  */
  YYSYMBOL_expr_discard_or_inline_decl_opt = 94, /* expr_discard_or_inline_decl_opt  */
  YYSYMBOL_stmt_braced = 95,               /* stmt_braced  */
  YYSYMBOL_scalar_var_init = 96,           /* scalar_var_init  */
  YYSYMBOL_var_decl = 97,                  /* var_decl  */
  YYSYMBOL_type_decl = 98,                 /* type_decl  */
  YYSYMBOL_type_params = 99,               /* type_params  */
  YYSYMBOL_type_spec_opt = 100,            /* type_spec_opt  */
  YYSYMBOL_type_spec = 101,                /* type_spec  */
  YYSYMBOL_complex_typespec = 102,         /* complex_typespec  */
  YYSYMBOL_var_typed = 103,                /* var_typed  */
  YYSYMBOL_struct_field = 104,             /* struct_field  */
  YYSYMBOL_struct_field_list = 105,        /* struct_field_list  */
  YYSYMBOL_struct_field_list_ne = 106,     /* struct_field_list_ne  */
  YYSYMBOL_interf_func = 107,              /* interf_func  */
  YYSYMBOL_interf_func_list = 108,         /* interf_func_list  */
  YYSYMBOL_var_decl_list = 109,            /* var_decl_list  */
  YYSYMBOL_type_decl_list = 110,           /* type_decl_list  */
  YYSYMBOL_param_list = 111,               /* param_list  */
  YYSYMBOL_param_list_ne = 112,            /* param_list_ne  */
  YYSYMBOL_arg_list = 113,                 /* arg_list  */
  YYSYMBOL_arg_list_ne = 114,              /* arg_list_ne  */
  YYSYMBOL_expr_opt = 115,                 /* expr_opt  */
  YYSYMBOL_stmt_list_opt = 116,            /* stmt_list_opt  */
  YYSYMBOL_stmt_list = 117,                /* stmt_list  */
  YYSYMBOL_aug_assign = 118,               /* aug_assign  */
  YYSYMBOL_pattern_basic = 119,            /* pattern_basic  */
  YYSYMBOL_pattern = 120,                  /* pattern  */
  YYSYMBOL_pattern_branch = 121,           /* pattern_branch  */
  YYSYMBOL_pattern_list = 122,             /* pattern_list  */
  YYSYMBOL_struct_field_init = 123,        /* struct_field_init  */
  YYSYMBOL_struct_field_init_list = 124,   /* struct_field_init_list  */
  YYSYMBOL_named_typespec = 125,           /* named_typespec  */
  YYSYMBOL_basic_expr = 126,               /* basic_expr  */
  YYSYMBOL_postfix_expr = 127,             /* postfix_expr  */
  YYSYMBOL_unary_expr = 128,               /* unary_expr  */
  YYSYMBOL_unary_op = 129,                 /* unary_op  */
  YYSYMBOL_mult_expr = 130,                /* mult_expr  */
  YYSYMBOL_add_expr = 131,                 /* add_expr  */
  YYSYMBOL_shift_expr = 132,               /* shift_expr  */
  YYSYMBOL_rel_expr = 133,                 /* rel_expr  */
  YYSYMBOL_eq_expr = 134,                  /* eq_expr  */
  YYSYMBOL_l_and_expr = 135,               /* l_and_expr  */
  YYSYMBOL_l_or_expr = 136,                /* l_or_expr  */
  YYSYMBOL_pipe_expr = 137,                /* pipe_expr  */
  YYSYMBOL_is_expr = 138,                  /* is_expr  */
  YYSYMBOL_assign_expr = 139,              /* assign_expr  */
  YYSYMBOL_expr = 140,                     /* expr  */
  YYSYMBOL_expr_discard = 141,             /* expr_discard  */
  YYSYMBOL_expr_discard_opt = 142,         /* expr_discard_opt  */
  YYSYMBOL_var = 143                       /* var  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   824

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  84
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  60
/* YYNRULES -- Number of rules.  */
#define YYNRULES  165
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  345

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   317


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    83,     2,
      70,    71,    39,    37,    76,    38,    81,    40,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    75,    69,
      46,    72,    45,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    77,     2,    78,     2,    79,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    73,    80,    74,    82,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    41,    42,    43,    44,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    68,    68,    72,    73,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   103,   107,
     111,   112,   116,   117,   120,   121,   124,   125,   129,   130,
     131,   135,   139,   143,   144,   145,   146,   147,   151,   155,
     165,   166,   170,   171,   175,   176,   177,   178,   179,   180,
     181,   190,   194,   198,   199,   202,   203,   207,   211,   212,
     216,   217,   220,   221,   224,   225,   229,   230,   234,   235,
     239,   240,   244,   245,   249,   250,   254,   255,   259,   260,
     261,   262,   266,   267,   268,   269,   273,   274,   278,   282,
     283,   287,   291,   292,   296,   297,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   318,
     319,   320,   321,   322,   323,   327,   328,   332,   333,   334,
     335,   336,   337,   341,   342,   343,   347,   348,   349,   353,
     354,   355,   359,   360,   361,   362,   363,   367,   368,   369,
     373,   374,   377,   378,   382,   383,   387,   388,   392,   393,
     394,   398,   402,   406,   407,   411
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUMBER", "IDENT",
  "STRING", "KWHILE", "KIF", "KPRINT", "KELSE", "KREAD", "KFOR", "KDO",
  "KVAR", "KFUNC", "KRETURN", "KBREAK", "KCONTINUE", "KCONST", "KTYPE",
  "KTYPEOF", "KSIZEOF", "KSTRUCT", "KBITSOF", "KNEW", "KASSERT", "KLOOP",
  "KMATCH", "KIS", "RANGE", "IRANGE", "KGLOBAL", "KIMPL", "KIN",
  "KFOREACH", "KINTERFACE", "KSELF", "'+'", "'-'", "'*'", "'/'", "GE",
  "LE", "EQ", "NE", "'>'", "'<'", "REF", "DEREF", "APL", "AMN", "AML",
  "ADV", "INC", "DEC", "AND", "OR", "SHL", "SHR", "ARROW", "STRUCTLIT",
  "GENINST", "PIPELINE", "CAST", "UMINUS", "VDECL", "SCOPE", "TUPLEASSIGN",
  "THEN", "';'", "'('", "')'", "'='", "'{'", "'}'", "':'", "','", "'['",
  "']'", "'_'", "'|'", "'.'", "'~'", "'&'", "$accept", "session", "stmts",
  "stmt", "func_signature", "func", "func_list", "tuple_assign_left",
  "tuple_assign_right", "expr_or_inline_decl",
  "expr_discard_or_inline_decl_opt", "stmt_braced", "scalar_var_init",
  "var_decl", "type_decl", "type_params", "type_spec_opt", "type_spec",
  "complex_typespec", "var_typed", "struct_field", "struct_field_list",
  "struct_field_list_ne", "interf_func", "interf_func_list",
  "var_decl_list", "type_decl_list", "param_list", "param_list_ne",
  "arg_list", "arg_list_ne", "expr_opt", "stmt_list_opt", "stmt_list",
  "aug_assign", "pattern_basic", "pattern", "pattern_branch",
  "pattern_list", "struct_field_init", "struct_field_init_list",
  "named_typespec", "basic_expr", "postfix_expr", "unary_expr", "unary_op",
  "mult_expr", "add_expr", "shift_expr", "rel_expr", "eq_expr",
  "l_and_expr", "l_or_expr", "pipe_expr", "is_expr", "assign_expr", "expr",
  "expr_discard", "expr_discard_opt", "var", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,    43,    45,    42,
      47,   292,   293,   294,   295,    62,    60,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,    59,
      40,    41,    61,   123,   125,    58,    44,    91,    93,    95,
     124,    46,   126,    38
};
#endif

#define YYPACT_NINF (-219)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-53)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -219,    32,   420,  -219,  -219,  -219,   -40,   -29,   741,   741,
     -21,   420,    53,    53,   741,   539,   -15,    53,    53,    10,
      12,    18,   741,    -2,    22,    53,  -219,  -219,  -219,  -219,
    -219,  -219,   215,   420,  -219,  -219,  -219,     2,  -219,  -219,
      26,  -219,    50,    83,   741,     7,    48,    64,    71,    62,
      63,    52,    78,    98,  -219,  -219,    82,  -219,   741,   578,
      77,   215,   420,    84,    85,   616,   153,  -219,    86,    91,
     -48,  -219,   101,  -219,  -219,   102,  -219,   104,    97,   105,
    -219,    89,    89,    89,   108,   420,   578,     0,   109,   110,
     128,   111,    30,   131,    -3,   132,   138,  -219,   106,   420,
     420,  -219,   741,  -219,  -219,   741,   741,    53,  -219,  -219,
    -219,  -219,   741,   741,  -219,   741,   741,   741,   741,   741,
     741,   741,   741,   741,   741,   741,   741,   741,   741,   741,
     655,  -219,   133,    53,   136,  -219,   578,   420,  -219,  -219,
      53,   146,  -219,   150,    53,  -219,    89,   693,   151,   154,
    -219,  -219,   741,    53,  -219,   156,    13,  -219,  -219,    24,
      25,  -219,   339,   155,    53,   164,   741,    53,   164,   157,
    -219,  -219,  -219,   741,  -219,  -219,    53,  -219,  -219,   158,
     420,   159,  -219,   149,   169,   168,   170,   160,   152,  -219,
    -219,  -219,  -219,  -219,  -219,     7,     7,    48,    48,    64,
      64,    64,    64,    71,    71,    62,    63,    52,  -219,   107,
     166,    63,   420,  -219,   174,   501,   181,  -219,   -10,   741,
     741,  -219,    30,   183,   179,   741,    53,   189,  -219,    89,
    -219,  -219,  -219,  -219,   191,   192,   164,   188,   195,   198,
      53,   196,  -219,   197,   202,   164,   199,   204,   200,  -219,
     138,  -219,  -219,    53,  -219,   741,   655,  -219,  -219,   741,
     741,   655,  -219,   151,   420,   266,   206,   741,   208,   209,
     276,   211,  -219,   210,   213,  -219,  -219,    30,   655,   164,
    -219,  -219,  -219,  -219,  -219,  -219,    89,  -219,  -219,  -219,
     741,  -219,  -219,   170,  -219,   166,    63,    63,   107,   216,
     420,   741,   218,   741,   222,  -219,   282,    53,   217,   -46,
     219,   220,   225,    30,   229,   227,   287,  -219,   216,   420,
    -219,   233,  -219,  -219,  -219,    89,   741,   655,  -219,  -219,
     236,   741,   234,  -219,   420,    30,  -219,  -219,  -219,  -219,
     227,   741,  -219,   232,  -219
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     2,     1,   106,   165,     0,     0,     0,     0,
       0,     0,     0,     0,    83,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   105,   127,   129,   130,
     131,     5,     0,    84,   128,   132,     3,     0,    24,    27,
     110,   119,   125,   133,     0,   136,   139,   142,   147,   150,
     152,   154,   156,   158,   161,   162,     0,   104,     0,     0,
       0,     0,     0,     0,     0,    38,     0,    44,    70,     0,
      50,    49,     0,    82,    15,     0,    17,     0,    72,     0,
      49,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    53,   110,     0,   104,    86,     0,    85,
      84,    29,    78,   120,   121,    78,     0,     0,    88,    89,
      90,    91,     0,     0,   126,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     6,     0,     0,     0,    36,     0,     0,    11,    12,
       0,     0,    39,     0,     0,     7,     0,     0,    43,     0,
      13,    16,     0,     0,     8,     0,     0,    53,    52,     0,
       0,    14,     0,     0,     0,     0,     0,    63,     0,     0,
      55,    56,    54,     0,   111,   112,     0,    41,    87,   162,
      85,     0,    79,   102,     0,    80,   104,     0,     0,   123,
     159,   160,   134,   135,   133,   137,   138,   140,   141,   146,
     145,   144,   143,   148,   149,   151,   153,   155,    95,    96,
     157,    92,     0,    37,    50,     0,     0,    40,    50,    83,
       0,    71,    51,     0,     0,     0,    74,     0,    73,     0,
     107,   108,   109,   115,     0,     0,    30,     0,     0,     0,
      65,     0,    64,     0,     0,    68,     0,     0,     0,    33,
      32,   113,   118,     0,   117,     0,     0,   124,   122,     0,
       0,     0,    18,     0,    84,    19,     0,     0,     0,     0,
       0,    45,    42,    76,     0,    75,    10,    48,     0,     0,
      31,    25,    58,    62,    66,    59,     0,    67,    69,    60,
       0,    57,   103,     0,    81,   101,    93,    94,    97,   162,
       0,     0,     0,   164,     0,    47,     0,     0,     0,     0,
      99,     0,     0,    61,     0,     0,     0,    20,     0,     0,
     163,     0,    23,    46,    77,     0,     0,     0,   116,    26,
       0,     0,     0,    22,     0,    28,    98,   100,     9,    35,
      34,     0,    21,     0,   114
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -219,  -219,  -219,     1,  -158,  -160,  -217,   134,   -22,   -64,
    -219,   271,   178,  -219,  -219,   237,    99,   -65,     5,  -218,
    -219,  -219,    72,  -219,    69,  -105,   163,  -219,    14,   214,
      67,   113,  -219,   -17,  -219,    66,  -126,  -219,     9,  -219,
      75,   -23,  -219,  -219,    70,  -219,    28,    23,    45,    31,
     207,   -51,   212,  -219,  -219,    37,    -8,   -63,  -219,     8
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    97,    37,    38,   237,    91,   314,   134,
     141,    39,    67,    68,    78,   149,   148,    92,   157,   239,
     240,   241,   242,   245,   246,    69,    79,   274,   275,   181,
     182,    72,    98,    99,   113,   209,   309,   310,   311,   183,
     184,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,   321,    57
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      63,    64,   142,    36,   210,   236,    73,    75,   273,    94,
     244,   164,    66,   326,    84,   -52,   156,   159,   160,   280,
      70,    71,   163,   267,    95,    77,    80,   146,   -52,   147,
      58,   170,     3,    87,   261,   217,   -52,    93,    94,   221,
      96,    59,   170,   170,   171,   137,   115,   116,   170,    65,
     132,   135,   172,    95,    76,   171,   171,     5,   158,   158,
     158,   171,   312,   172,   172,   146,    93,   147,   162,   172,
     102,    85,   216,   165,   -52,   100,   236,   206,   135,   211,
      81,   222,    82,   180,   230,   117,   118,   244,    83,   273,
     173,   179,    86,     5,   185,   231,   232,   185,   188,   102,
     178,   173,   173,   103,   104,   125,   126,   173,   128,    88,
     186,    89,   121,   122,   114,   189,   123,   124,   127,   236,
     105,   119,   120,   158,    90,    26,   130,   106,   135,   179,
     295,   107,   108,   109,   110,   111,   259,   260,   178,   224,
     129,   214,   197,   198,   227,   195,   196,   136,   218,   190,
     191,   131,    70,   138,   139,   112,   203,   204,   238,   143,
     145,    80,   144,   178,   277,   248,   199,   200,   201,   202,
     150,   151,   235,   153,   154,   243,   152,   161,    13,   166,
     177,   178,   169,   167,   250,   192,   193,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   168,   174,   175,   212,   211,   158,   215,   296,   297,
     211,    73,   269,   262,   176,   219,   265,   272,     4,     5,
     220,   313,    60,   225,   226,   253,   234,   211,   229,   247,
     258,   257,   251,   252,   243,    88,    19,    89,    20,    21,
     320,    23,    24,   254,   255,   256,   261,   185,   243,   146,
      90,    26,   266,    27,    28,   270,   299,   271,   276,   302,
     335,   293,   281,   158,   278,   279,   282,   283,    29,    30,
     285,   287,   286,   289,   290,   300,   211,   303,   291,   301,
     304,   305,   315,   306,   308,    61,   307,   323,    62,   319,
     316,   322,   325,   318,   328,   327,   332,    34,    35,   329,
     330,   317,   158,   331,   334,   338,   344,   341,   101,   339,
     249,   213,   284,   263,   288,   243,   228,   155,   336,   187,
     333,   324,   294,   340,     0,     0,   194,   298,   292,   194,
     194,   194,   268,   343,   205,   342,   337,     0,     0,     0,
       0,   207,     4,     5,     0,     6,     7,     8,   194,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,     0,
      19,     0,    20,    21,    22,    23,    24,     0,     0,     0,
       0,    25,     0,     0,     0,    26,     0,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,    30,     0,     0,     0,   194,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
       0,     0,    33,   233,     0,     0,     0,     0,     0,     0,
       0,    34,    35,     4,     5,     0,     6,     7,     8,     0,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
       0,    19,     0,    20,    21,    22,    23,    24,     0,     0,
       0,     0,    25,     0,     0,     0,    26,     0,    27,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,    30,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    31,
      32,     0,     0,    33,     0,     0,     0,     0,     0,     0,
       0,     0,    34,    35,     4,     5,     0,     6,     7,     8,
       0,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     0,    19,     0,    20,    21,    22,    23,    24,     0,
       0,     0,     0,    25,     0,     0,     0,    26,     0,    27,
      28,     0,     4,     5,     0,     0,    60,     0,     0,     0,
       0,     0,     0,     0,    29,    30,     0,     0,     0,     0,
      19,     0,    20,    21,     0,    23,    24,     0,     0,     0,
      31,    32,     0,     0,   264,    26,     0,    27,    28,     0,
       0,     4,     5,    34,    35,    60,     0,     0,     0,     0,
       0,   133,    29,    30,     0,     0,     0,     0,     0,    19,
       0,    20,    21,     0,    23,    24,     0,     0,    74,    61,
       0,     0,    62,     0,    26,     0,    27,    28,     0,     4,
       5,    34,    35,    60,     0,     0,     0,     0,     0,   140,
       0,    29,    30,     0,     0,     0,     0,    19,     0,    20,
      21,     0,    23,    24,     0,     0,     0,     0,    61,     0,
       0,    62,    26,     0,    27,    28,     0,     0,     4,     5,
      34,    35,    60,     0,     0,     0,     0,     0,     0,    29,
      30,     0,     0,     0,     0,     0,    19,     0,    20,    21,
       0,    23,    24,     0,     0,     0,    61,     0,     0,    62,
       0,    26,     0,    27,    28,     0,     4,     5,    34,    35,
      60,     0,     0,     0,     0,     0,     0,     0,    29,    30,
       0,     0,     0,     0,    19,     0,    20,    21,     0,    23,
      24,     0,     0,     0,     0,    61,     0,     0,    62,    26,
       0,    27,    28,     0,   208,     0,     0,    34,    35,     0,
       0,     0,     0,     0,     4,     5,    29,    30,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    19,    61,    20,    21,    62,    23,    24,     0,
       0,   223,     0,     0,     0,    34,    35,    26,     0,    27,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,    30,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    61,     0,     0,    62,     0,     0,     0,     0,     0,
       0,     0,     0,    34,    35
};

static const yytype_int16 yycheck[] =
{
       8,     9,    65,     2,   130,   165,    14,    15,   226,    32,
     168,    11,    11,    59,    22,    18,    81,    82,    83,   236,
      12,    13,    86,    33,    32,    17,    18,    75,    31,    77,
      70,    18,     0,    25,    80,   140,    39,    32,    61,   144,
      32,    70,    18,    18,    31,    62,    39,    40,    18,    70,
      58,    59,    39,    61,    69,    31,    31,     4,    81,    82,
      83,    31,   279,    39,    39,    75,    61,    77,    85,    39,
      73,    73,   136,    73,    77,    73,   236,   128,    86,   130,
      70,   146,    70,   100,    71,    37,    38,   245,    70,   307,
      77,    99,    70,     4,   102,    71,    71,   105,   106,    73,
      99,    77,    77,    53,    54,    43,    44,    77,    56,    20,
     102,    22,    41,    42,    44,   107,    45,    46,    55,   279,
      70,    57,    58,   146,    35,    36,    28,    77,   136,   137,
     256,    81,    49,    50,    51,    52,    29,    30,   137,   147,
      62,   133,   119,   120,   152,   117,   118,    70,   140,   112,
     113,    69,   144,    69,    69,    72,   125,   126,   166,     6,
      69,   153,    76,   162,   229,   173,   121,   122,   123,   124,
      69,    69,   164,    76,    69,   167,    72,    69,    14,    70,
      74,   180,    71,    73,   176,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,    73,    71,    71,    71,   256,   229,    71,   259,   260,
     261,   219,   220,   212,    76,    69,   215,   225,     3,     4,
      70,   286,     7,    72,    70,    76,    71,   278,    72,    72,
      78,    71,    74,    74,   226,    20,    21,    22,    23,    24,
     303,    26,    27,    74,    76,    75,    80,   255,   240,    75,
      35,    36,    71,    38,    39,    72,   264,    78,    69,   267,
     325,   253,    74,   286,    73,    73,    71,    69,    53,    54,
      74,    69,    75,    74,    70,     9,   327,    69,    78,    73,
      71,     5,   290,    72,    71,    70,    76,     5,    73,    71,
      74,    69,    75,   301,    74,    76,     9,    82,    83,    74,
      71,   300,   325,    76,    71,    69,    74,    73,    37,   331,
     176,   133,   240,   214,   245,   307,   153,    80,   326,   105,
     319,   307,   255,   331,    -1,    -1,   256,   261,   253,   259,
     260,   261,   219,   341,   127,   334,   327,    -1,    -1,    -1,
      -1,   129,     3,     4,    -1,     6,     7,     8,   278,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    -1,
      21,    -1,    23,    24,    25,    26,    27,    -1,    -1,    -1,
      -1,    32,    -1,    -1,    -1,    36,    -1,    38,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    54,    -1,    -1,    -1,   327,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,
      -1,    -1,    73,    74,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    83,     3,     4,    -1,     6,     7,     8,    -1,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      -1,    21,    -1,    23,    24,    25,    26,    27,    -1,    -1,
      -1,    -1,    32,    -1,    -1,    -1,    36,    -1,    38,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,
      70,    -1,    -1,    73,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    83,     3,     4,    -1,     6,     7,     8,
      -1,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    -1,    21,    -1,    23,    24,    25,    26,    27,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    -1,    36,    -1,    38,
      39,    -1,     3,     4,    -1,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    53,    54,    -1,    -1,    -1,    -1,
      21,    -1,    23,    24,    -1,    26,    27,    -1,    -1,    -1,
      69,    70,    -1,    -1,    73,    36,    -1,    38,    39,    -1,
      -1,     3,     4,    82,    83,     7,    -1,    -1,    -1,    -1,
      -1,    13,    53,    54,    -1,    -1,    -1,    -1,    -1,    21,
      -1,    23,    24,    -1,    26,    27,    -1,    -1,    69,    70,
      -1,    -1,    73,    -1,    36,    -1,    38,    39,    -1,     3,
       4,    82,    83,     7,    -1,    -1,    -1,    -1,    -1,    13,
      -1,    53,    54,    -1,    -1,    -1,    -1,    21,    -1,    23,
      24,    -1,    26,    27,    -1,    -1,    -1,    -1,    70,    -1,
      -1,    73,    36,    -1,    38,    39,    -1,    -1,     3,     4,
      82,    83,     7,    -1,    -1,    -1,    -1,    -1,    -1,    53,
      54,    -1,    -1,    -1,    -1,    -1,    21,    -1,    23,    24,
      -1,    26,    27,    -1,    -1,    -1,    70,    -1,    -1,    73,
      -1,    36,    -1,    38,    39,    -1,     3,     4,    82,    83,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    54,
      -1,    -1,    -1,    -1,    21,    -1,    23,    24,    -1,    26,
      27,    -1,    -1,    -1,    -1,    70,    -1,    -1,    73,    36,
      -1,    38,    39,    -1,    79,    -1,    -1,    82,    83,    -1,
      -1,    -1,    -1,    -1,     3,     4,    53,    54,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    21,    70,    23,    24,    73,    26,    27,    -1,
      -1,    78,    -1,    -1,    -1,    82,    83,    36,    -1,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    53,    54,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    -1,    -1,    73,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    82,    83
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    85,    86,     0,     3,     4,     6,     7,     8,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    21,
      23,    24,    25,    26,    27,    32,    36,    38,    39,    53,
      54,    69,    70,    73,    82,    83,    87,    88,    89,    95,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   143,    70,    70,
       7,    70,    73,   140,   140,    70,    87,    96,    97,   109,
     143,   143,   115,   140,    69,   140,    69,   143,    98,   110,
     143,    70,    70,    70,   140,    73,    70,   143,    20,    22,
      35,    91,   101,   102,   125,   140,   143,    87,   116,   117,
      73,    95,    73,    53,    54,    70,    77,    81,    49,    50,
      51,    52,    72,   118,   128,    39,    40,    37,    38,    57,
      58,    41,    42,    45,    46,    43,    44,    55,    56,    62,
      28,    69,   140,    13,    93,   140,    70,   117,    69,    69,
      13,    94,   141,     6,    76,    69,    75,    77,   100,    99,
      69,    69,    72,    76,    69,    99,   101,   102,   125,   101,
     101,    69,   117,    93,    11,    73,    70,    73,    73,    71,
      18,    31,    39,    77,    71,    71,    76,    74,    87,   140,
     117,   113,   114,   123,   124,   140,   143,   113,   140,   143,
     139,   139,   128,   128,   128,   130,   130,   131,   131,   132,
     132,   132,   132,   133,   133,   134,   135,   136,    79,   119,
     120,   135,    71,    96,   143,    71,    93,   109,   143,    69,
      70,   109,   101,    78,   140,    72,    70,   140,   110,    72,
      71,    71,    71,    74,    71,   143,    89,    90,   140,   103,
     104,   105,   106,   143,    88,   107,   108,    72,   140,    91,
     143,    74,    74,    76,    74,    76,    75,    71,    78,    29,
      30,    80,    87,   100,    73,    87,    71,    33,   115,   140,
      72,    78,   140,   103,   111,   112,    69,   101,    73,    73,
      90,    74,    71,    69,   106,    74,    75,    69,   108,    74,
      70,    78,   124,   143,   114,   120,   135,   135,   119,   140,
       9,    73,   140,    69,    71,     5,    72,    76,    71,   120,
     121,   122,    90,   101,    92,   140,    74,    87,   140,    71,
     141,   142,    69,     5,   112,    75,    59,    76,    74,    74,
      71,    76,     9,    87,    71,   101,   140,   122,    69,    92,
     140,    73,    87,   140,    74
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    84,    85,    86,    86,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    88,    89,
      90,    90,    91,    91,    92,    92,    93,    93,    94,    94,
      94,    95,    96,    97,    97,    97,    97,    97,    98,    99,
     100,   100,   101,   101,   102,   102,   102,   102,   102,   102,
     102,   103,   104,   105,   105,   106,   106,   107,   108,   108,
     109,   109,   110,   110,   111,   111,   112,   112,   113,   113,
     114,   114,   115,   115,   116,   116,   117,   117,   118,   118,
     118,   118,   119,   119,   119,   119,   120,   120,   121,   122,
     122,   123,   124,   124,   125,   125,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   126,   127,
     127,   127,   127,   127,   127,   128,   128,   129,   129,   129,
     129,   129,   129,   130,   130,   130,   131,   131,   131,   132,
     132,   132,   133,   133,   133,   133,   133,   134,   134,   134,
     135,   135,   136,   136,   137,   137,   138,   138,   139,   139,
     139,   140,   141,   142,   142,   143
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     0,     1,     2,     3,     3,     8,
       5,     3,     3,     3,     3,     2,     3,     2,     5,     5,
       7,     9,     8,     7,     1,     5,     7,     1,     8,     2,
       1,     2,     3,     3,     3,     3,     1,     2,     0,     1,
       2,     3,     4,     2,     1,     4,     6,     5,     4,     0,
       0,     2,     1,     1,     2,     2,     2,     4,     4,     4,
       4,     3,     2,     0,     1,     1,     2,     2,     1,     2,
       1,     3,     1,     3,     0,     1,     1,     3,     0,     1,
       1,     3,     1,     0,     0,     1,     1,     2,     1,     1,
       1,     1,     1,     3,     3,     1,     1,     3,     3,     1,
       3,     3,     1,     3,     1,     1,     1,     4,     4,     4,
       1,     3,     3,     4,    11,     4,     7,     4,     4,     1,
       2,     2,     4,     3,     4,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     1,     3,     3,     1,
       3,     3,     1,     3,     3,     3,     3,     1,     3,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       3,     1,     1,     1,     0,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YY_LOCATION_PRINT
#  if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#   define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

#  else
#   define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#  endif
# endif /* !defined YY_LOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */
#line 28 "syntax.y"
{
	yylloc = (YYLTYPE){1, 1, 1, 1, malloc(1024)};
	yylloc.code[0] = 0;
}

#line 1589 "syntax.tab.c"

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* session: stmts  */
#line 68 "syntax.y"
                                { fprintf(stderr, "Bye.\n");  exit(0); }
#line 1801 "syntax.tab.c"
    break;

  case 3: /* stmts: stmts stmt  */
#line 72 "syntax.y"
                                { produce_code((yyvsp[0].node)); free_node((yyvsp[0].node)); }
#line 1807 "syntax.tab.c"
    break;

  case 5: /* stmt: ';'  */
#line 77 "syntax.y"
                                                                                                                                                { (yyval.node) = make_node(';', 0); }
#line 1813 "syntax.tab.c"
    break;

  case 6: /* stmt: expr_discard ';'  */
#line 78 "syntax.y"
                                                                                                                                                                { (yyval.node) = (yyvsp[-1].node); }
#line 1819 "syntax.tab.c"
    break;

  case 7: /* stmt: KVAR var_decl_list ';'  */
#line 79 "syntax.y"
                                                                                                                                                        { (yyval.node) = (yyvsp[-1].node); }
#line 1825 "syntax.tab.c"
    break;

  case 8: /* stmt: KTYPE type_decl_list ';'  */
#line 80 "syntax.y"
                                                                                                                                                        { (yyval.node) = (yyvsp[-1].node); }
#line 1831 "syntax.tab.c"
    break;

  case 9: /* stmt: '(' tuple_assign_left ')' '=' '(' tuple_assign_right ')' ';'  */
#line 81 "syntax.y"
                                                                                                                        { (yyval.node) = make_node(TUPLEASSIGN, 2, (yyvsp[-6].node), (yyvsp[-2].node)); }
#line 1837 "syntax.tab.c"
    break;

  case 10: /* stmt: KCONST var '=' expr ';'  */
#line 82 "syntax.y"
                                                                                                                                                        { (yyval.node) = make_node(KCONST, 2, (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 1843 "syntax.tab.c"
    break;

  case 11: /* stmt: KPRINT expr ';'  */
#line 83 "syntax.y"
                                                                                                                                                { (yyval.node) = make_node(KPRINT, 1, (yyvsp[-1].node)); }
#line 1849 "syntax.tab.c"
    break;

  case 12: /* stmt: KREAD expr ';'  */
#line 84 "syntax.y"
                                                                                                                                                { (yyval.node) = make_node(KREAD, 1, (yyvsp[-1].node)); }
#line 1855 "syntax.tab.c"
    break;

  case 13: /* stmt: KRETURN expr_opt ';'  */
#line 85 "syntax.y"
                                                                                                                                        { (yyval.node) = make_node(KRETURN, 1, (yyvsp[-1].node)); }
#line 1861 "syntax.tab.c"
    break;

  case 14: /* stmt: KASSERT expr ';'  */
#line 86 "syntax.y"
                                                                                                                                                { (yyval.node) = make_node(KASSERT, 1, (yyvsp[-1].node)); }
#line 1867 "syntax.tab.c"
    break;

  case 15: /* stmt: KBREAK ';'  */
#line 87 "syntax.y"
                                                                                                                                                                        { (yyval.node) = make_node(KBREAK, 0); }
#line 1873 "syntax.tab.c"
    break;

  case 16: /* stmt: KBREAK expr ';'  */
#line 88 "syntax.y"
                                                                                                                                                                { (yyval.node) = make_node(KBREAK, 1, (yyvsp[-1].node)); }
#line 1879 "syntax.tab.c"
    break;

  case 17: /* stmt: KCONTINUE ';'  */
#line 89 "syntax.y"
                                                                                                                                                                        { (yyval.node) = make_node(KCONTINUE, 0); }
#line 1885 "syntax.tab.c"
    break;

  case 18: /* stmt: KWHILE '(' expr ')' stmt  */
#line 90 "syntax.y"
                                                                                                                                                { (yyval.node) = make_node(KFOR, 4, NULL, (yyvsp[-2].node), NULL, (yyvsp[0].node)); }
#line 1891 "syntax.tab.c"
    break;

  case 19: /* stmt: KIF '(' expr_or_inline_decl ')' stmt  */
#line 91 "syntax.y"
                                                                                                                        { (yyval.node) = make_scope(make_node(KIF, 3, (yyvsp[-2].node), (yyvsp[0].node), NULL)); }
#line 1897 "syntax.tab.c"
    break;

  case 20: /* stmt: KIF '(' expr_or_inline_decl ')' stmt KELSE stmt  */
#line 92 "syntax.y"
                                                                                                                                { (yyval.node) = make_scope(make_node(KIF, 3, (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node))); }
#line 1903 "syntax.tab.c"
    break;

  case 21: /* stmt: KFOR '(' expr_discard_or_inline_decl_opt ';' expr_opt ';' expr_discard_opt ')' stmt  */
#line 93 "syntax.y"
                                                                                                { (yyval.node) = make_scope(make_node(KFOR, 4, (yyvsp[-6].node), (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node))); }
#line 1909 "syntax.tab.c"
    break;

  case 22: /* stmt: KFOR '(' KVAR var KIN expr ')' stmt  */
#line 94 "syntax.y"
                                                                                                                                                { (yyval.node) = make_scope(make_node(KFOREACH, 3, (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node))); }
#line 1915 "syntax.tab.c"
    break;

  case 23: /* stmt: KDO stmt KWHILE '(' expr ')' ';'  */
#line 95 "syntax.y"
                                                                                                                                                { (yyval.node) = make_node(KDO, 2, (yyvsp[-5].node), (yyvsp[-2].node)); }
#line 1921 "syntax.tab.c"
    break;

  case 24: /* stmt: func  */
#line 96 "syntax.y"
                                                                                                                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 1927 "syntax.tab.c"
    break;

  case 25: /* stmt: KIMPL var '{' func_list '}'  */
#line 97 "syntax.y"
                                                                                                                                                        { (yyval.node) = make_node(KIMPL, 2, (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 1933 "syntax.tab.c"
    break;

  case 26: /* stmt: KIMPL var KFOR var '{' func_list '}'  */
#line 98 "syntax.y"
                                                                                                                                                { (yyval.node) = make_node(KIMPL, 3, (yyvsp[-3].node), (yyvsp[-1].node), (yyvsp[-5].node)); }
#line 1939 "syntax.tab.c"
    break;

  case 27: /* stmt: stmt_braced  */
#line 99 "syntax.y"
                                                                                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 1945 "syntax.tab.c"
    break;

  case 28: /* func_signature: KFUNC var type_params '(' param_list ')' ':' type_spec  */
#line 103 "syntax.y"
                                                                                                                                { (yyval.node) = make_node(KFUNC, 5, (yyvsp[-6].node), (yyvsp[-3].node), NULL, (yyvsp[0].node), (yyvsp[-5].node)); }
#line 1951 "syntax.tab.c"
    break;

  case 29: /* func: func_signature stmt_braced  */
#line 107 "syntax.y"
                                                                                                                                                        { (yyval.node) = (yyvsp[-1].node); OPER_OPERANDS((yyval.node))[2] = (yyvsp[0].node); }
#line 1957 "syntax.tab.c"
    break;

  case 30: /* func_list: func  */
#line 111 "syntax.y"
                                                                { (yyval.node) = make_list((yyvsp[0].node)); }
#line 1963 "syntax.tab.c"
    break;

  case 31: /* func_list: func func_list  */
#line 112 "syntax.y"
                                                        { (yyval.node) = prepend_list((yyvsp[0].node), (yyvsp[-1].node)); }
#line 1969 "syntax.tab.c"
    break;

  case 32: /* tuple_assign_left: var ',' var  */
#line 116 "syntax.y"
                                                        { (yyval.node) = prepend_list(make_list((yyvsp[0].node)), (yyvsp[-2].node)); }
#line 1975 "syntax.tab.c"
    break;

  case 33: /* tuple_assign_left: var ',' tuple_assign_left  */
#line 117 "syntax.y"
                                                { (yyval.node) = prepend_list((yyvsp[0].node), (yyvsp[-2].node)); }
#line 1981 "syntax.tab.c"
    break;

  case 34: /* tuple_assign_right: expr ',' expr  */
#line 120 "syntax.y"
                                                        { (yyval.node) = prepend_list(make_list((yyvsp[0].node)), (yyvsp[-2].node)); }
#line 1987 "syntax.tab.c"
    break;

  case 35: /* tuple_assign_right: expr ',' tuple_assign_right  */
#line 121 "syntax.y"
                                        { (yyval.node) = prepend_list((yyvsp[0].node), (yyvsp[-2].node)); }
#line 1993 "syntax.tab.c"
    break;

  case 36: /* expr_or_inline_decl: expr  */
#line 124 "syntax.y"
                                                                { (yyval.node) = (yyvsp[0].node); }
#line 1999 "syntax.tab.c"
    break;

  case 37: /* expr_or_inline_decl: KVAR scalar_var_init  */
#line 125 "syntax.y"
                                                { (yyval.node) = (yyvsp[0].node); }
#line 2005 "syntax.tab.c"
    break;

  case 38: /* expr_discard_or_inline_decl_opt: %empty  */
#line 129 "syntax.y"
                                                                        { (yyval.node) = NULL; }
#line 2011 "syntax.tab.c"
    break;

  case 39: /* expr_discard_or_inline_decl_opt: expr_discard  */
#line 130 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2017 "syntax.tab.c"
    break;

  case 40: /* expr_discard_or_inline_decl_opt: KVAR var_decl_list  */
#line 131 "syntax.y"
                                                { (yyval.node) = (yyvsp[0].node); AST_CLEAN_STACK((yyvsp[0].node)) = true; }
#line 2023 "syntax.tab.c"
    break;

  case 41: /* stmt_braced: '{' stmt_list_opt '}'  */
#line 135 "syntax.y"
                                                                                                                                { (yyval.node) = make_scope((yyvsp[-1].node)); }
#line 2029 "syntax.tab.c"
    break;

  case 42: /* scalar_var_init: var type_spec_opt '=' expr  */
#line 139 "syntax.y"
                                                                                                                                        { (yyval.node) = make_node(';', 2, make_node(KVAR, 2, (yyvsp[-3].node), (yyvsp[-2].node) ? (yyvsp[-2].node) : make_node(KTYPEOF, 1, (yyvsp[0].node))), make_node('=', 2, (yyvsp[-3].node), (yyvsp[0].node))); }
#line 2035 "syntax.tab.c"
    break;

  case 43: /* var_decl: var type_spec_opt  */
#line 143 "syntax.y"
                                                                                                                                                { (yyval.node) = make_node(KVAR, 2, (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2041 "syntax.tab.c"
    break;

  case 44: /* var_decl: scalar_var_init  */
#line 144 "syntax.y"
                                                                                                                                                { (yyval.node) = (yyvsp[0].node); AST_CLEAN_STACK(OPER_OPERANDS((yyvsp[0].node))[1]) = true; }
#line 2047 "syntax.tab.c"
    break;

  case 45: /* var_decl: var '[' expr ']'  */
#line 145 "syntax.y"
                                                                                                                                                { (yyval.node) = make_node(KVAR, 3, (yyvsp[-3].node), (yyvsp[-1].node), NULL); }
#line 2053 "syntax.tab.c"
    break;

  case 46: /* var_decl: var '[' expr ']' '=' STRING  */
#line 146 "syntax.y"
                                                                                                                                        { (yyval.node) = make_node(KVAR, 3, (yyvsp[-5].node), (yyvsp[-3].node), make_ident((yyvsp[0].var))); }
#line 2059 "syntax.tab.c"
    break;

  case 47: /* var_decl: var '[' ']' '=' STRING  */
#line 147 "syntax.y"
                                                                                                                                        { (yyval.node) = make_node(KVAR, 3, (yyvsp[-4].node), make_number(strlen((yyvsp[0].var))), make_ident((yyvsp[0].var))); }
#line 2065 "syntax.tab.c"
    break;

  case 48: /* type_decl: var type_params '=' type_spec  */
#line 151 "syntax.y"
                                                                                                                                        { (yyval.node) = make_node(KTYPE, 3, (yyvsp[-3].node), (yyvsp[0].node), (yyvsp[-2].node)); }
#line 2071 "syntax.tab.c"
    break;

  case 49: /* type_params: %empty  */
#line 155 "syntax.y"
                                                                        { (yyval.node) = NULL; }
#line 2077 "syntax.tab.c"
    break;

  case 50: /* type_spec_opt: %empty  */
#line 165 "syntax.y"
                                                                        { (yyval.node) = NULL; }
#line 2083 "syntax.tab.c"
    break;

  case 51: /* type_spec_opt: ':' type_spec  */
#line 166 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2089 "syntax.tab.c"
    break;

  case 52: /* type_spec: named_typespec  */
#line 170 "syntax.y"
                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2095 "syntax.tab.c"
    break;

  case 53: /* type_spec: complex_typespec  */
#line 171 "syntax.y"
                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2101 "syntax.tab.c"
    break;

  case 54: /* complex_typespec: type_spec '*'  */
#line 175 "syntax.y"
                                                                        { (yyval.node) = make_node('*', 1, (yyvsp[-1].node)); }
#line 2107 "syntax.tab.c"
    break;

  case 55: /* complex_typespec: type_spec KCONST  */
#line 176 "syntax.y"
                                                                        { (yyval.node) = make_node(KCONST, 1, (yyvsp[-1].node)); }
#line 2113 "syntax.tab.c"
    break;

  case 56: /* complex_typespec: type_spec KGLOBAL  */
#line 177 "syntax.y"
                                                                        { (yyval.node) = make_node(KGLOBAL, 1, (yyvsp[-1].node)); }
#line 2119 "syntax.tab.c"
    break;

  case 57: /* complex_typespec: type_spec '[' expr ']'  */
#line 178 "syntax.y"
                                                                { (yyval.node) = make_node('[', 2, (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2125 "syntax.tab.c"
    break;

  case 58: /* complex_typespec: KTYPEOF '(' expr ')'  */
#line 179 "syntax.y"
                                                                { (yyval.node) = make_node(KTYPEOF, 1, (yyvsp[-1].node)); }
#line 2131 "syntax.tab.c"
    break;

  case 59: /* complex_typespec: KSTRUCT '{' struct_field_list '}'  */
#line 180 "syntax.y"
                                                        { (yyval.node) = make_node(KSTRUCT, 1, (yyvsp[-1].node)); }
#line 2137 "syntax.tab.c"
    break;

  case 60: /* complex_typespec: KINTERFACE '{' interf_func_list '}'  */
#line 181 "syntax.y"
                                                { (yyval.node) = make_node(KINTERFACE, 1, (yyvsp[-1].node)); }
#line 2143 "syntax.tab.c"
    break;

  case 61: /* var_typed: var ':' type_spec  */
#line 190 "syntax.y"
                                                                { (yyval.node) = make_node(':', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2149 "syntax.tab.c"
    break;

  case 62: /* struct_field: var_typed ';'  */
#line 194 "syntax.y"
                                                                { (yyval.node) = (yyvsp[-1].node); }
#line 2155 "syntax.tab.c"
    break;

  case 63: /* struct_field_list: %empty  */
#line 198 "syntax.y"
                                                                                { (yyval.node) = make_empty_list(); }
#line 2161 "syntax.tab.c"
    break;

  case 64: /* struct_field_list: struct_field_list_ne  */
#line 199 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2167 "syntax.tab.c"
    break;

  case 65: /* struct_field_list_ne: struct_field  */
#line 202 "syntax.y"
                                                                { (yyval.node) = make_list((yyvsp[0].node)); }
#line 2173 "syntax.tab.c"
    break;

  case 66: /* struct_field_list_ne: struct_field struct_field_list_ne  */
#line 203 "syntax.y"
                                                { (yyval.node) = prepend_list((yyvsp[0].node), (yyvsp[-1].node)); }
#line 2179 "syntax.tab.c"
    break;

  case 67: /* interf_func: func_signature ';'  */
#line 207 "syntax.y"
                                                        { (yyval.node) = (yyvsp[-1].node); }
#line 2185 "syntax.tab.c"
    break;

  case 68: /* interf_func_list: interf_func  */
#line 211 "syntax.y"
                                                                { (yyval.node) = make_list((yyvsp[0].node)); }
#line 2191 "syntax.tab.c"
    break;

  case 69: /* interf_func_list: interf_func interf_func_list  */
#line 212 "syntax.y"
                                                { (yyval.node) = prepend_list((yyvsp[0].node), (yyvsp[-1].node)); }
#line 2197 "syntax.tab.c"
    break;

  case 70: /* var_decl_list: var_decl  */
#line 216 "syntax.y"
                                                                { (yyval.node) = (yyvsp[0].node); }
#line 2203 "syntax.tab.c"
    break;

  case 71: /* var_decl_list: var_decl ',' var_decl_list  */
#line 217 "syntax.y"
                                        { (yyval.node) = make_node(';', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2209 "syntax.tab.c"
    break;

  case 72: /* type_decl_list: type_decl  */
#line 220 "syntax.y"
                                                                { (yyval.node) = (yyvsp[0].node); }
#line 2215 "syntax.tab.c"
    break;

  case 73: /* type_decl_list: type_decl ',' type_decl_list  */
#line 221 "syntax.y"
                                        { (yyval.node) = make_node(';', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2221 "syntax.tab.c"
    break;

  case 74: /* param_list: %empty  */
#line 224 "syntax.y"
                                                                        { (yyval.node) = NULL; }
#line 2227 "syntax.tab.c"
    break;

  case 75: /* param_list: param_list_ne  */
#line 225 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2233 "syntax.tab.c"
    break;

  case 76: /* param_list_ne: var_typed  */
#line 229 "syntax.y"
                                                                { (yyval.node) = make_list((yyvsp[0].node)); }
#line 2239 "syntax.tab.c"
    break;

  case 77: /* param_list_ne: var_typed ',' param_list_ne  */
#line 230 "syntax.y"
                                        { (yyval.node) = prepend_list((yyvsp[0].node), (yyvsp[-2].node)); }
#line 2245 "syntax.tab.c"
    break;

  case 78: /* arg_list: %empty  */
#line 234 "syntax.y"
                                                                        { (yyval.node) = NULL; }
#line 2251 "syntax.tab.c"
    break;

  case 79: /* arg_list: arg_list_ne  */
#line 235 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2257 "syntax.tab.c"
    break;

  case 80: /* arg_list_ne: expr  */
#line 239 "syntax.y"
                                                                { (yyval.node) = make_list((yyvsp[0].node)); }
#line 2263 "syntax.tab.c"
    break;

  case 81: /* arg_list_ne: expr ',' arg_list_ne  */
#line 240 "syntax.y"
                                                { (yyval.node) = prepend_list((yyvsp[0].node), (yyvsp[-2].node)); }
#line 2269 "syntax.tab.c"
    break;

  case 82: /* expr_opt: expr  */
#line 244 "syntax.y"
                                                { (yyval.node) = (yyvsp[0].node); }
#line 2275 "syntax.tab.c"
    break;

  case 83: /* expr_opt: %empty  */
#line 245 "syntax.y"
                                                        { (yyval.node) = NULL; }
#line 2281 "syntax.tab.c"
    break;

  case 84: /* stmt_list_opt: %empty  */
#line 249 "syntax.y"
                                                                        { (yyval.node) = NULL; }
#line 2287 "syntax.tab.c"
    break;

  case 85: /* stmt_list_opt: stmt_list  */
#line 250 "syntax.y"
                                                                { (yyval.node) = (yyvsp[0].node); }
#line 2293 "syntax.tab.c"
    break;

  case 86: /* stmt_list: stmt  */
#line 254 "syntax.y"
                                        { (yyval.node) = (yyvsp[0].node); }
#line 2299 "syntax.tab.c"
    break;

  case 87: /* stmt_list: stmt_list stmt  */
#line 255 "syntax.y"
                                        { (yyval.node) = make_node(';', 2, (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2305 "syntax.tab.c"
    break;

  case 88: /* aug_assign: APL  */
#line 259 "syntax.y"
                                                                { (yyval.chr) = '+'; }
#line 2311 "syntax.tab.c"
    break;

  case 89: /* aug_assign: AMN  */
#line 260 "syntax.y"
                                                                { (yyval.chr) = '-'; }
#line 2317 "syntax.tab.c"
    break;

  case 90: /* aug_assign: AML  */
#line 261 "syntax.y"
                                                                { (yyval.chr) = '*'; }
#line 2323 "syntax.tab.c"
    break;

  case 91: /* aug_assign: ADV  */
#line 262 "syntax.y"
                                                                { (yyval.chr) = '/'; }
#line 2329 "syntax.tab.c"
    break;

  case 92: /* pattern_basic: l_and_expr  */
#line 266 "syntax.y"
                                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2335 "syntax.tab.c"
    break;

  case 93: /* pattern_basic: pattern_basic RANGE l_and_expr  */
#line 267 "syntax.y"
                                                                                        { (yyval.node) = make_node(RANGE, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2341 "syntax.tab.c"
    break;

  case 94: /* pattern_basic: pattern_basic IRANGE l_and_expr  */
#line 268 "syntax.y"
                                                                                        { (yyval.node) = make_node(RANGE, 2, (yyvsp[-2].node), (yyvsp[0].node)); AST_DATA((yyval.node)) = (void*) 1; }
#line 2347 "syntax.tab.c"
    break;

  case 95: /* pattern_basic: '_'  */
#line 269 "syntax.y"
                                                                                                                { (yyval.node) = NULL; }
#line 2353 "syntax.tab.c"
    break;

  case 96: /* pattern: pattern_basic  */
#line 273 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2359 "syntax.tab.c"
    break;

  case 97: /* pattern: pattern '|' pattern_basic  */
#line 274 "syntax.y"
                                                { (yyval.node) = make_node('|', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2365 "syntax.tab.c"
    break;

  case 98: /* pattern_branch: pattern ARROW expr  */
#line 278 "syntax.y"
                                                { (yyval.node) = make_node(ARROW, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2371 "syntax.tab.c"
    break;

  case 99: /* pattern_list: pattern_branch  */
#line 282 "syntax.y"
                                                                { (yyval.node) = make_list((yyvsp[0].node)); }
#line 2377 "syntax.tab.c"
    break;

  case 100: /* pattern_list: pattern_branch ',' pattern_list  */
#line 283 "syntax.y"
                                                { (yyval.node) = prepend_list((yyvsp[0].node), (yyvsp[-2].node)); }
#line 2383 "syntax.tab.c"
    break;

  case 101: /* struct_field_init: var ':' pattern  */
#line 287 "syntax.y"
                                                                        { (yyval.node) = make_node('=', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2389 "syntax.tab.c"
    break;

  case 102: /* struct_field_init_list: struct_field_init  */
#line 291 "syntax.y"
                                                                                        { (yyval.node) = make_list((yyvsp[0].node)); }
#line 2395 "syntax.tab.c"
    break;

  case 103: /* struct_field_init_list: struct_field_init ',' struct_field_init_list  */
#line 292 "syntax.y"
                                                        { (yyval.node) = prepend_list((yyvsp[0].node), (yyvsp[-2].node)); }
#line 2401 "syntax.tab.c"
    break;

  case 104: /* named_typespec: var  */
#line 296 "syntax.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 2407 "syntax.tab.c"
    break;

  case 105: /* named_typespec: KSELF  */
#line 297 "syntax.y"
                                                                                { (yyval.node) = make_node(KSELF, 0); }
#line 2413 "syntax.tab.c"
    break;

  case 106: /* basic_expr: NUMBER  */
#line 302 "syntax.y"
                                                                                                                                                { (yyval.node) = make_number_sized((yyvsp[0].number).value, (yyvsp[0].number).size); }
#line 2419 "syntax.tab.c"
    break;

  case 107: /* basic_expr: KSIZEOF '(' type_spec ')'  */
#line 303 "syntax.y"
                                                                                                                        { (yyval.node) = make_node(KSIZEOF, 1, (yyvsp[-1].node)); }
#line 2425 "syntax.tab.c"
    break;

  case 108: /* basic_expr: KBITSOF '(' type_spec ')'  */
#line 304 "syntax.y"
                                                                                                                        { (yyval.node) = make_node(KBITSOF, 1, (yyvsp[-1].node)); }
#line 2431 "syntax.tab.c"
    break;

  case 109: /* basic_expr: KNEW '(' type_spec ')'  */
#line 305 "syntax.y"
                                                                                                                                { (yyval.node) = make_node(KNEW, 1, (yyvsp[-1].node)); }
#line 2437 "syntax.tab.c"
    break;

  case 110: /* basic_expr: named_typespec  */
#line 306 "syntax.y"
                                                                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2443 "syntax.tab.c"
    break;

  case 111: /* basic_expr: '(' complex_typespec ')'  */
#line 307 "syntax.y"
                                                                                                                                { (yyval.node) = (yyvsp[-1].node); }
#line 2449 "syntax.tab.c"
    break;

  case 112: /* basic_expr: '(' expr ')'  */
#line 308 "syntax.y"
                                                                                                                                        { (yyval.node) = (yyvsp[-1].node); }
#line 2455 "syntax.tab.c"
    break;

  case 113: /* basic_expr: '{' stmt_list expr '}'  */
#line 309 "syntax.y"
                                                                                                                                { (yyval.node) = make_scope(make_node('{', 2, (yyvsp[-2].node), (yyvsp[-1].node))); }
#line 2461 "syntax.tab.c"
    break;

  case 114: /* basic_expr: KIF '(' expr_or_inline_decl ')' '{' expr '}' KELSE '{' expr '}'  */
#line 310 "syntax.y"
                                                                                { (yyval.node) = make_scope(make_node(KIF, 3, (yyvsp[-8].node), (yyvsp[-5].node), (yyvsp[-1].node))); }
#line 2467 "syntax.tab.c"
    break;

  case 115: /* basic_expr: KLOOP '{' stmt_list '}'  */
#line 311 "syntax.y"
                                                                                                                        { (yyval.node) = make_node(KLOOP, 1, (yyvsp[-1].node)); }
#line 2473 "syntax.tab.c"
    break;

  case 116: /* basic_expr: KMATCH '(' expr_or_inline_decl ')' '{' pattern_list '}'  */
#line 312 "syntax.y"
                                                                                        { (yyval.node) = make_node(KMATCH, 2, (yyvsp[-4].node), (yyvsp[-1].node)); }
#line 2479 "syntax.tab.c"
    break;

  case 117: /* basic_expr: named_typespec '{' struct_field_init_list '}'  */
#line 313 "syntax.y"
                                                                                                        { (yyval.node) = make_node(STRUCTLIT, 2, (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2485 "syntax.tab.c"
    break;

  case 118: /* basic_expr: named_typespec '{' arg_list '}'  */
#line 314 "syntax.y"
                                                                                                                { (yyval.node) = make_node(STRUCTLIT, 3, (yyvsp[-3].node), (yyvsp[-1].node), NULL); }
#line 2491 "syntax.tab.c"
    break;

  case 119: /* postfix_expr: basic_expr  */
#line 318 "syntax.y"
                                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 2497 "syntax.tab.c"
    break;

  case 120: /* postfix_expr: postfix_expr INC  */
#line 319 "syntax.y"
                                                                        { (yyval.node) = make_node(INC, 2, (yyvsp[-1].node), NULL); }
#line 2503 "syntax.tab.c"
    break;

  case 121: /* postfix_expr: postfix_expr DEC  */
#line 320 "syntax.y"
                                                                        { (yyval.node) = make_node(DEC, 2, (yyvsp[-1].node), NULL); }
#line 2509 "syntax.tab.c"
    break;

  case 122: /* postfix_expr: postfix_expr '[' expr ']'  */
#line 321 "syntax.y"
                                                                                { (yyval.node) = make_node(DEREF, 1, make_node('+', 2, (yyvsp[-3].node), (yyvsp[-1].node))); }
#line 2515 "syntax.tab.c"
    break;

  case 123: /* postfix_expr: postfix_expr '.' var  */
#line 322 "syntax.y"
                                                                                        { (yyval.node) = make_node('.', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2521 "syntax.tab.c"
    break;

  case 124: /* postfix_expr: postfix_expr '(' arg_list ')'  */
#line 323 "syntax.y"
                                                                                { (yyval.node) = make_node('(', 3, (yyvsp[-3].node), NULL, (yyvsp[-1].node)); }
#line 2527 "syntax.tab.c"
    break;

  case 125: /* unary_expr: postfix_expr  */
#line 327 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2533 "syntax.tab.c"
    break;

  case 126: /* unary_expr: unary_op unary_expr  */
#line 328 "syntax.y"
                                                { (yyval.node) = make_node((yyvsp[-1].chr), 1, (yyvsp[0].node)); }
#line 2539 "syntax.tab.c"
    break;

  case 127: /* unary_op: '-'  */
#line 332 "syntax.y"
                        { (yyval.chr) = UMINUS; }
#line 2545 "syntax.tab.c"
    break;

  case 128: /* unary_op: '~'  */
#line 333 "syntax.y"
                        { (yyval.chr) = '~'; }
#line 2551 "syntax.tab.c"
    break;

  case 129: /* unary_op: '*'  */
#line 334 "syntax.y"
                        { (yyval.chr) = DEREF; }
#line 2557 "syntax.tab.c"
    break;

  case 130: /* unary_op: INC  */
#line 335 "syntax.y"
                        { (yyval.chr) = INC; }
#line 2563 "syntax.tab.c"
    break;

  case 131: /* unary_op: DEC  */
#line 336 "syntax.y"
                        { (yyval.chr) = DEC; }
#line 2569 "syntax.tab.c"
    break;

  case 132: /* unary_op: '&'  */
#line 337 "syntax.y"
                        { (yyval.chr) = REF; }
#line 2575 "syntax.tab.c"
    break;

  case 133: /* mult_expr: unary_expr  */
#line 341 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2581 "syntax.tab.c"
    break;

  case 134: /* mult_expr: mult_expr '*' unary_expr  */
#line 342 "syntax.y"
                                        { (yyval.node) = make_node('*', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2587 "syntax.tab.c"
    break;

  case 135: /* mult_expr: mult_expr '/' unary_expr  */
#line 343 "syntax.y"
                                        { (yyval.node) = make_node('/', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2593 "syntax.tab.c"
    break;

  case 136: /* add_expr: mult_expr  */
#line 347 "syntax.y"
                                                                { (yyval.node) = (yyvsp[0].node); }
#line 2599 "syntax.tab.c"
    break;

  case 137: /* add_expr: add_expr '+' mult_expr  */
#line 348 "syntax.y"
                                        { (yyval.node) = make_node('+', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2605 "syntax.tab.c"
    break;

  case 138: /* add_expr: add_expr '-' mult_expr  */
#line 349 "syntax.y"
                                        { (yyval.node) = make_node('-', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2611 "syntax.tab.c"
    break;

  case 139: /* shift_expr: add_expr  */
#line 353 "syntax.y"
                                                                { (yyval.node) = (yyvsp[0].node); }
#line 2617 "syntax.tab.c"
    break;

  case 140: /* shift_expr: shift_expr SHL add_expr  */
#line 354 "syntax.y"
                                                { (yyval.node) = make_node(SHL, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2623 "syntax.tab.c"
    break;

  case 141: /* shift_expr: shift_expr SHR add_expr  */
#line 355 "syntax.y"
                                                { (yyval.node) = make_node(SHR, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2629 "syntax.tab.c"
    break;

  case 142: /* rel_expr: shift_expr  */
#line 359 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2635 "syntax.tab.c"
    break;

  case 143: /* rel_expr: rel_expr '<' shift_expr  */
#line 360 "syntax.y"
                                        { (yyval.node) = make_node('<', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2641 "syntax.tab.c"
    break;

  case 144: /* rel_expr: rel_expr '>' shift_expr  */
#line 361 "syntax.y"
                                        { (yyval.node) = make_node('>', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2647 "syntax.tab.c"
    break;

  case 145: /* rel_expr: rel_expr LE shift_expr  */
#line 362 "syntax.y"
                                        { (yyval.node) = make_node(LE, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2653 "syntax.tab.c"
    break;

  case 146: /* rel_expr: rel_expr GE shift_expr  */
#line 363 "syntax.y"
                                        { (yyval.node) = make_node(GE, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2659 "syntax.tab.c"
    break;

  case 147: /* eq_expr: rel_expr  */
#line 367 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2665 "syntax.tab.c"
    break;

  case 148: /* eq_expr: eq_expr EQ rel_expr  */
#line 368 "syntax.y"
                                                { (yyval.node) = make_node(EQ, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2671 "syntax.tab.c"
    break;

  case 149: /* eq_expr: eq_expr NE rel_expr  */
#line 369 "syntax.y"
                                                { (yyval.node) = make_node(NE, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2677 "syntax.tab.c"
    break;

  case 150: /* l_and_expr: eq_expr  */
#line 373 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2683 "syntax.tab.c"
    break;

  case 151: /* l_and_expr: l_and_expr AND eq_expr  */
#line 374 "syntax.y"
                                        { (yyval.node) = make_node(AND, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2689 "syntax.tab.c"
    break;

  case 152: /* l_or_expr: l_and_expr  */
#line 377 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2695 "syntax.tab.c"
    break;

  case 153: /* l_or_expr: l_or_expr OR l_and_expr  */
#line 378 "syntax.y"
                                        { (yyval.node) = make_node(OR, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2701 "syntax.tab.c"
    break;

  case 154: /* pipe_expr: l_or_expr  */
#line 382 "syntax.y"
                                                                { (yyval.node) = (yyvsp[0].node); }
#line 2707 "syntax.tab.c"
    break;

  case 155: /* pipe_expr: pipe_expr PIPELINE l_or_expr  */
#line 383 "syntax.y"
                                        { (yyval.node) = make_node('(', 3, (yyvsp[0].node), NULL, make_list((yyvsp[-2].node))); }
#line 2713 "syntax.tab.c"
    break;

  case 156: /* is_expr: pipe_expr  */
#line 387 "syntax.y"
                                                                { (yyval.node) = (yyvsp[0].node); }
#line 2719 "syntax.tab.c"
    break;

  case 157: /* is_expr: is_expr KIS pattern  */
#line 388 "syntax.y"
                                                { (yyval.node) = make_node(KIS, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2725 "syntax.tab.c"
    break;

  case 158: /* assign_expr: is_expr  */
#line 392 "syntax.y"
                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 2731 "syntax.tab.c"
    break;

  case 159: /* assign_expr: unary_expr '=' assign_expr  */
#line 393 "syntax.y"
                                                        { (yyval.node) = make_node('=', 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2737 "syntax.tab.c"
    break;

  case 160: /* assign_expr: unary_expr aug_assign assign_expr  */
#line 394 "syntax.y"
                                                { (yyval.node) = make_node('=', 2, (yyvsp[-2].node), make_node((yyvsp[-1].chr), 2, (yyvsp[-2].node), (yyvsp[0].node))); }
#line 2743 "syntax.tab.c"
    break;

  case 161: /* expr: assign_expr  */
#line 398 "syntax.y"
                                                { (yyval.node) = (yyvsp[0].node); }
#line 2749 "syntax.tab.c"
    break;

  case 162: /* expr_discard: expr  */
#line 402 "syntax.y"
                                                        { (yyval.node) = (yyvsp[0].node); AST_CLEAN_STACK((yyval.node)) = true; }
#line 2755 "syntax.tab.c"
    break;

  case 163: /* expr_discard_opt: expr_discard  */
#line 406 "syntax.y"
                                                { (yyval.node) = (yyvsp[0].node); }
#line 2761 "syntax.tab.c"
    break;

  case 164: /* expr_discard_opt: %empty  */
#line 407 "syntax.y"
                                                                { (yyval.node) = NULL; }
#line 2767 "syntax.tab.c"
    break;

  case 165: /* var: IDENT  */
#line 411 "syntax.y"
                                { (yyval.node) = make_ident((yyvsp[0].var)); }
#line 2773 "syntax.tab.c"
    break;


#line 2777 "syntax.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 414 "syntax.y"

void yyerror(const char *s)     { error_msg(NULL, "%s\n",s); exit(1); }
int  yywrap(void)               { return 1;          } // to avoid linking with -ldl
bool write_c = false;
int  main(int argc, char* argv[]) {
  extern FILE *yyin;
  yyin = stdin;
  if (argc >= 2) {
    yyin = fopen(argv[1], "r");
    if (!yyin) {
      fprintf(stderr, "%s: cannot open input file '%s'\n", *argv, argv[1]);
      exit(1);
    }
    for (int i = 2; i < argc; i++) {
      if (!strcmp(argv[i], "-c")) {
		printf("# Write C code\n");
	    write_c = true;
	  }
    }
  }
  return yyparse();
}
