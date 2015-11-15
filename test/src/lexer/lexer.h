
#ifndef _LEXER_H_INCLUDED
#define _LEXER_H_INCLUDED

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void *yyscan_t;
#endif
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
struct Extra {
	int continued;
	int cur_line;
	int cur_column;
};

void init_scanner(char *program, yyscan_t *scanner, struct Extra *extra);
void destroy_scanner(yyscan_t scanner);



#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
#define YYTOKENTYPE
  enum yytokentype
  {
	  TERM = 1, //терминальные символы грамматики реализации
	  SP_NON_TERM = 2,
	  SP_SEM_RULE = 3,
	  SP_DECL_TERM = 4,
	  SP_DECL_NONTERM = 5,
	  SP_L_SQ_BRACKET = 6,
	  SP_R_SQ_BRACKET = 7,
	  SP_L_BRACE = 8,
	  SP_R_BRACE = 9,
	  SP_L_PARENT = 10,
	  SP_R_PARENT = 11,
	  SP_DOT = 12,
	  SP_DIR_SLASH = 13,
	  SP_EQUAL = 14,
	  SP_MULTI = 15,
	  SP_COMMA = 16,
	  SP_TYPE = 17,
	  SP_L_TR_BRACKET = 18,
	  SP_R_TR_BRACKET = 19,
	  NUMBER = 20,
	  PRINT = 21,
	  PLUS = 22,
	  MINUS = 23,
	  SEMICOLON =24,
	  MULTI = 25,
	  DIVIDE = 26,
	  ERROR = 27
  };
  union TYPES {
	  string  *__string__;
	  int __int__;
	  char __char__;
  };

#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef struct YYSTYPE YYSTYPE;
struct YYSTYPE
{
	int tag;
	int mark;
	union TYPES *attr;
};
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
int yylex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner);
string tokenToStr(YYSTYPE *token, YYLTYPE *coord);

#endif

