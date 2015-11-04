
%option reentrant noyywrap bison-bridge bison-locations
%option extra-type="struct Extra *"

%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h" 

#define YY_USER_ACTION                                             \
	{                                                          \
		int i;                                             \
		struct Extra *extra = yyextra;                     \
		if (! extra->continued) {                          \
			yylloc->first_line = extra->cur_line;      \
			yylloc->first_column = extra->cur_column;  \
		}                                                  \
		extra->continued = 0;                              \
		for (i = 0; i < yyleng; i++)                       \
		{                                                  \
			if (yytext[i] == '\n')                     \
			{                                          \
				extra->cur_line++;                 \
				extra->cur_column = 1;             \
			}                                          \
			else                                       \
				extra->cur_column++;               \
			}                                          \
		                                                   \
		yylloc->last_line = extra->cur_line;               \
		yylloc->last_column = extra->cur_column;           \
		yylval->num = NULL;                               \
	}

void yyerror(YYLTYPE *loc, yyscan_t scanner, long env[26], char *msg)
{
	printf("Error (%d,%d): %s\n", loc->first_line, loc->first_column, msg);
}

%}   
%x SEMRULE SPSEMRULE
WITHOUT_ATTR_TERMS '<'|'>'|','|[A-Z][A-Z_]*
%%
[\n\t ]+
@TERM					  {
							yylval->tag = TERM;
							yylval->mark = TERM;
							yylval->value = yytext;
							return TERM;
						  }

@NONTERM				  {
							yylval->tag = TERM;
							yylval->mark = SP_NON_TERM;
							yylval->value = yytext;
							return TERM;
						  }

@SEMRULE				  {
							yylval->tag = TERM;
							yylval->mark = SP_SEM_RULE;
							yylval->value = yytext;
							return TERM;
						  }

'\%term'				  {
							yylval->tag = TERM;
							yylval->mark = SP_DECL_TERM;
							yylval->value = yytext;
							return TERM;
						  }

'\%nonterm'	   			  {
							yylval->tag = TERM;
							yylval->mark = SP_DECL_NONTERM;
							yylval->value = yytext;
							return TERM;
						  }

'\['	   			  	  {
							yylval->tag = TERM;
							yylval->mark = SP_L_SQ_BRACKET;
							yylval->value = yytext;
							return TERM;
						  }

'\]'	   			  	  {
							yylval->tag = TERM;
							yylval->mark = SP_R_SQ_BRACKET;
							yylval->value = yytext;
							return TERM;
						  }

'\{'					  {
							yylval->tag = TERM;
							yylval->mark = SP_L_BRACE;
							yylval->value = yytext;
							return TERM;
						  }
						  
'\}'					  {
							yylval->tag = TERM;
							yylval->mark = SP_R_BRACE;
							yylval->value = yytext;
							return TERM;
						  }
						  
'\('					  {
							yylval->tag = TERM;
							yylval->mark = SP_L_PARENT;
							yylval->value = yytext;
							return TERM;
						  }
						  
'\)'					  {
							yylval->tag = TERM;
							yylval->mark = SP_R_PARENT;
							yylval->value = yytext;
							return TERM;
						  }
						  
'\.'					  {
							yylval->tag = TERM;
							yylval->mark = SP_DOT;
							yylval->value = yytext;
							return TERM;
						  }
						  
'\|'					  {
							yylval->tag = TERM;
							yylval->mark = SP_DIR_SLASH;
							yylval->value = yytext;
							return TERM;
						  }
						  
'\='					  {
							yylval->tag = TERM;
							yylval->mark = SP_EQUAL;
							yylval->value = yytext;
							return TERM;
						  }	
						  
'\*'					  {
							yylval->tag = TERM;
							yylval->mark = SP_MULTI;
							yylval->value = yytext;
							return TERM;
						  }


						  
'<'			  		      {
							yylval->tag = TERM;
							yylval->mark = SP_L_TR_BRACKET;
							yylval->value = yytext;
							return TERM;
						  }
						  
'>'			  		      {
							yylval->tag = TERM;
							yylval->mark = SP_R_TR_BRACKET;
							yylval->value = yytext;
							return TERM;
						  }	
						  
','			  		      {
							yylval->tag = TERM;
							yylval->mark = SP_COMMA;
							yylval->value = yytext;
							return TERM;
						  }
						  
'__cpchar__'|'__int__'|'__char__'	 {
										yylval->tag = TERM;
										yylval->mark = SP_TYPE;
										yylval->value = yytext;
										return TERM;
									  }							  						  								  


\%\{				{
						BEGIN(SPSEMRULE); 
						yyextra->continued=1;
						yylval->value = (char*)malloc(1024*sizeof(*yylval->value));
						strcat(yylval->value, yytext);
					}

<SPSEMRULE>\%\}		{
						BEGIN(0); 
						strcat(yylval->value, yytext);
						yylval->tag=SP_SEM_RULE;
						return SP_SEM_RULE;
					}

<SPSEMRULE>.|\n		{
						yyextra->continued = 1;
						strcat(yylval->value, yytext);
					}

__int__|__char__|__cpchar__	 yylval->tag=SP_TYPE; return SP_TYPE;


[a-z][a-z_]*		yylval->tag=SP_NON_TERM; return SP_NON_TERM;



\%term				yylval->tag=SP_DECL_TERM; return SP_DECL_TERM;
\%nonterm			yylval->tag=SP_DECL_NONTERM; return SP_DECL_NONTERM;
\[					yylval->tag=SP_L_SQ_BRACKET; return SP_L_SQ_BRACKET;
\]					yylval->tag=SP_R_SQ_BRACKET; return SP_R_SQ_BRACKET;
\{					yylval->tag=SP_L_BRACE; return SP_L_BRACE;
\}					yylval->tag=SP_R_BRACE; return SP_R_BRACE;
\(					yylval->tag=SP_L_PARENT; return SP_L_PARENT;
\)					yylval->tag=SP_R_PARENT; return SP_R_PARENT;
\.					yylval->tag=SP_DOT; return SP_DOT;
\|					yylval->tag=SP_DIR_SLASH; return SP_DIR_SLASH;
\=					yylval->tag=SP_EQUAL; return SP_EQUAL;
\*					yylval->tag=SP_MULTI; return SP_MULTI;
\,					yylval->tag=SP_COMMA; return SP_COMMA;
\<					yylval->tag=SP_L_TR_BRACKET; return SP_L_TR_BRACKET;
\>					yylval->tag=SP_R_TR_BRACKET; return SP_R_TR_BRACKET;

.					{
						printf("ERROR AT %d:%d - %d:%d\n",  yylloc->first_line, yylloc->first_column, yylloc->last_line, yylloc->last_column);
					}


%%
char const* Domains[] = {
		  "",
		  "TERM",
		  "SP_NON_TERM",
		  "SP_SEM_RULE",
		  "SP_DECL_TERM",
		  "SP_DECL_NONTERM",
		  "SP_L_SQ_BRACKET",
		  "SP_R_SQ_BRACKET",
		  "SP_L_BRACE",
		  "SP_R_BRACE",
		  "SP_L_PARENT",
		  "SP_R_PARENT",
		  "SP_DOT",
		  "SP_DIR_SLASH",
		  "SP_EQUAL",
		  "SP_MULTI",
		  "SP_COMMA",
		  "SP_TYPE",
		  "SP_L_TR_BRACKET",
		  "SP_R_TR_BRACKET",
		  "SP_CONST_CHAR_POINTER_TYPE",
		  "SP_INT_TYPE",
		  "SP_CHAR_TYPE"
};

void init_scanner(char *program, yyscan_t *scanner, struct Extra *extra)
{
	extra->continued = 0;
	extra->cur_line = 1;
	extra->cur_column = 1;
	yylex_init(scanner);
	yylex_init_extra(extra, scanner);
	yy_scan_string(program, *scanner);
}

void destroy_scanner(yyscan_t scanner)
{
	yylex_destroy(scanner);
}
