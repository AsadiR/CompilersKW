
%option reentrant noyywrap bison-bridge bison-locations
%option extra-type="struct Extra *"
%option nounput
%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h" 

#define YY_USER_ACTION                                     \
	{                                                      \
		int i;                                             \
		struct Extra *extra = yyextra;                     \
		if (! extra->continued) {						   \
			yylval->attr = NULL;			   			   \
			yylval->mark = 0;							   \
			yylval->attr_type = NULL;					   \
			yylloc->first_line = extra->cur_line;          \
			yylloc->first_column = extra->cur_column;      \
		}                                                  \
		extra->continued = 0;                              \
		for (i = 0; i < (int)yyleng; i++)                  \
		{                                                  \
			if (yytext[i] == '\n')                         \
			{                                              \
				extra->cur_line++;                         \
				extra->cur_column = 1;               	   \
			}                                              \
			else                                           \
				extra->cur_column++;                       \
			}                                              \
		                                                   \
		yylloc->last_line = extra->cur_line;               \
		yylloc->last_column = extra->cur_column;           \
	}

void yyerror(YYLTYPE *loc, yyscan_t scanner, long env[26], char *msg)
{
	printf("Error (%d,%d): %s\n", loc->first_line, loc->first_column, msg);
}

void createStrAttr(YYSTYPE *val, char* text) {
		val->attr = new TYPES;
		val->attr->__string__ = new string(text);
}

%} 
  
%x SPSEMRULE

%%
[\n\t ]+
@TERM					  {
							yylval->tag = TERM;
							yylval->mark = TERM;
							createStrAttr(yylval, yytext);
							return TERM;
						  }

@NONTERM				  {
							yylval->tag = TERM;
							yylval->mark = SP_NON_TERM;
							createStrAttr(yylval, yytext);
							return TERM;
						  }

@SEMRULE				  {
							yylval->tag = TERM;
							yylval->mark = SP_SEM_RULE;
							createStrAttr(yylval, yytext);
							return TERM;
						  }

@TYPE	 				  {
							yylval->tag = TERM;
							yylval->mark = SP_TYPE;
							createStrAttr(yylval, yytext);
							return TERM;
		 				  }								
						  

'\%term'				  {
							yylval->tag = TERM;
							yylval->mark = SP_DECL_TERM;
							createStrAttr(yylval, yytext);
							return TERM;
						  }

'\%nonterm'	   			  {
							yylval->tag = TERM;
							yylval->mark = SP_DECL_NONTERM;
							createStrAttr(yylval, yytext);
							return TERM;
						  }

'\['	   			  	  {
							yylval->tag = TERM;
							yylval->mark = SP_L_SQ_BRACKET;
							createStrAttr(yylval, yytext);
							return TERM;
						  }

'\]'	   			  	  {
							yylval->tag = TERM;
							createStrAttr(yylval, yytext);
							return TERM;
						  }

'\{'					  {
							yylval->tag = TERM;
							yylval->mark = SP_L_BRACE;
							createStrAttr(yylval, yytext);
							return TERM;
						  }
						  
'\}'					  {
							yylval->tag = TERM;
							yylval->mark = SP_R_BRACE;
							createStrAttr(yylval, yytext);
							return TERM;
						  }
						  
'\('					  {
							yylval->tag = TERM;
							yylval->mark = SP_L_PARENT;
							createStrAttr(yylval, yytext);
							return TERM;
						  }
						  
'\)'					  {
							yylval->tag = TERM;
							yylval->mark = SP_R_PARENT;
							createStrAttr(yylval, yytext);
							return TERM;
						  }
						  
'\.'					  {
							yylval->tag = TERM;
							yylval->mark = SP_DOT;
							createStrAttr(yylval, yytext);
							return TERM;
						  }
						  
'\|'					  {
							yylval->tag = TERM;
							yylval->mark = SP_DIR_SLASH;
							createStrAttr(yylval, yytext);
							return TERM;
						  }
						  
'\='					  {
							yylval->tag = TERM;
							yylval->mark = SP_EQUAL;
							createStrAttr(yylval, yytext);
							return TERM;
						  }	
						  
'\*'					  {
							yylval->tag = TERM;
							yylval->mark = SP_MULTI;
							createStrAttr(yylval, yytext);
							return TERM;
						  }


						  
'<'			  		      {
							yylval->tag = TERM;
							yylval->mark = SP_L_TR_BRACKET;
							createStrAttr(yylval, yytext);
							return TERM;
						  }
						  
'>'			  		      {
							yylval->tag = TERM;
							yylval->mark = SP_R_TR_BRACKET;
							createStrAttr(yylval, yytext);
							return TERM;
						  }	
						  
','			  		      {
							yylval->tag = TERM;
							yylval->mark = SP_COMMA;
							createStrAttr(yylval, yytext);
							return TERM;
						  }				  						  								  
	

\%\{				{
						BEGIN(SPSEMRULE);
						yyextra->continued=1;
						createStrAttr(yylval, yytext);
					}

<SPSEMRULE>\%\}		{
						BEGIN(0); 
						*(yylval->attr->__string__) += string(yytext);
						yylval->tag=SP_SEM_RULE;
						return SP_SEM_RULE;
					}

<SPSEMRULE>.|\n		{
						yyextra->continued = 1;
						*(yylval->attr->__string__) += string(yytext);
					}

__int__				{
						yylval->tag = SP_TYPE;
						createStrAttr(yylval, yytext);
						yylval->attr_type = new string("int");
						return SP_TYPE;
					}
				
__char__			{
						yylval->tag = SP_TYPE;
						createStrAttr(yylval, yytext);
						yylval->attr_type = new string("char");
						return SP_TYPE;
					}
				
__string__			{
						yylval->tag = SP_TYPE;
						createStrAttr(yylval, yytext);
						yylval->attr_type = new string("string");
						return SP_TYPE;
					}


[a-z][a-z_]*		{
						yylval->tag=SP_NON_TERM;
						createStrAttr(yylval, yytext);
						return SP_NON_TERM;
					}



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
<<EOF>>				yylval->tag=0; return 0;

@NUMBER				{
							yylval->tag = TERM;
							yylval->mark = NUMBER;
							createStrAttr(yylval, yytext);
							return TERM;
					}

@PRINT				{
							yylval->tag = TERM;
							yylval->mark = PRINT;
							createStrAttr(yylval, yytext);
							return TERM;
					}
					
@SEMICOLON			{
							yylval->tag = TERM;
							yylval->mark = SEMICOLON;
							createStrAttr(yylval, yytext);
							return TERM;
					}
					
@COMMA				{
							yylval->tag = TERM;
							yylval->mark = SP_COMMA;
							createStrAttr(yylval, yytext);
							return TERM;
					}
					
@PLUS			    {
							yylval->tag = TERM;
							yylval->mark = PLUS;
							createStrAttr(yylval, yytext);
							return TERM;
					}
					
@MINUS				{
							yylval->tag = TERM;
							yylval->mark = MINUS;
							createStrAttr(yylval, yytext);
							return TERM;
					}
					
@MULTI				{
							yylval->tag = TERM;
							yylval->mark = SP_MULTI;
							createStrAttr(yylval, yytext);
							return TERM;
					}
					
@DIVIDE				{
							yylval->tag = TERM;
							yylval->mark = DIVIDE;
							createStrAttr(yylval, yytext);
							return TERM;
					}
					
@LPAREN				{
							yylval->tag = TERM;
							yylval->mark = SP_L_PARENT;
							createStrAttr(yylval, yytext);
							return TERM;
					}
					
@RPAREN				{
							yylval->tag = TERM;
							yylval->mark = SP_R_PARENT;
							createStrAttr(yylval, yytext);
							return TERM;
					}
					
					
					

[0-9][0-9]*			{
							yylval->tag = NUMBER;
							yylval->attr = new TYPES;
							yylval->attr->__int__ = atoi( yytext );
							return TERM;							
					}
					
\+					return PLUS;
\-					return MINUS;
\/					return DIVIDE;
\;					return SEMICOLON;
PRINT				return PRINT;					
					
.					{
						yylval->tag = ERROR;
						yylval->attr = new TYPES;
						string s = string ("ERROR AT") + tokenToStr(yylval, yylloc);
						yylval->attr->__string__ = new string(s.c_str());
						return ERROR;
					}


%%
const char* Domains[] = {
		  /*0*/
		  "<<EOF>>",
		  /*1*/
		  "TERM",
		  /*2*/
		  "SP_NON_TERM",
		  /*3*/
		  "SP_SEM_RULE",
		  /*4*/
		  "SP_DECL_TERM",
		  /*5*/
		  "SP_DECL_NONTERM",
		  /*6*/
		  "SP_L_SQ_BRACKET",
		  /*7*/
		  "SP_R_SQ_BRACKET",
		  /*8*/
		  "SP_L_BRACE",
		  /*9*/
		  "SP_R_BRACE",
		  /*10*/
		  "SP_L_PARENT",
		  /*11*/
		  "SP_R_PARENT",
		  /*12*/
		  "SP_DOT",
		  /*13*/
		  "SP_DIR_SLASH",
		  /*14*/
		  "SP_EQUAL",
		  /*15*/
		  "SP_MULTI",
		  /*16*/
		  "SP_COMMA",
		  /*17*/
		  "SP_TYPE",
		  /*18*/
		  "SP_L_TR_BRACKET",
		  /*19*/
		  "SP_R_TR_BRACKET",
		  /*20*/
		  "NUMBER",
		  /*21*/
		  "PRINT",
		  /*22*/
		  "PLUS",
		  /*23*/
		  "MINUS",
		  /*24*/
		  "SEMICOLON",
		  /*25*/
		  "MULTI",
		  /*26*/
		  "DIVIDE",
		  /*27*/
		  "ERROR"
				  
};

string tokenToStr(YYSTYPE *token, YYLTYPE *coord) {
	stringstream ss;
	ss << string(Domains[token->tag])
				<< "( " << coord->first_line << "," << coord->first_column << " - "
				<< coord->last_line << "," << coord->last_column << " )";
	return ss.str();
}

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

