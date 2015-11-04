/*
 * Parser.cpp
 *
 *  Created on: 11.10.2015
 *      Author: ugin
 */

#include "Parser.h"
#include "lexer/lexer.h"

using namespace std;

extern char const* Domains[];

Parser::Parser(vector<YYSTYPE> t) {
	num = 0;
	this->tokens = t;
	sym = &tokens[0];
	root = new Node();
	printf("Parser Created\n");
	printf("firstSym:%s\n", Domains[sym->tag]);
}

Node* Parser::parse() {
	parseGrammar(root);
	return root;
}

Parser::~Parser() {
	// TODO Auto-generated destructor stub
}

//grammar = [@SEMRULE] init rule {rule} [@SEMRULE].
void Parser::parseGrammar(Node *parent) {
	if (sym->tag==SP_SEM_RULE) {
		expectedToken(parent->addChild(new Node), SP_SEM_RULE);
	}
	parseInit(parent->addChild(new Node));
	printf("ParsingOfRules...\n");
	parseRule(parent->addChild(new Node));
	while (sym->tag==SP_NON_TERM) {
		parseRule(parent->addChild(new Node));
	}
	if (sym->tag==SP_SEM_RULE)
		expectedToken(parent->addChild(new Node), SP_SEM_RULE);
}
//init = [term_decl] [nonterm_decl].
void Parser::parseInit(Node *parent) {
	if (sym->tag == SP_DECL_TERM)
		parseTermDecl(parent->addChild(new Node));
	if (sym->tag == SP_DECL_NONTERM)
		parseNonTermDecl(parent->addChild(new Node));
}
//term_decl = '%term' [attr] @TERM {',' [attr]  @TERM} '.' .
void Parser::parseTermDecl(Node *parent) {
	expectedToken(parent->addChild(new Node), SP_DECL_TERM);
	if (sym->tag==SP_L_TR_BRACKET) {
		parseAttribute(parent->addChild(new Node));
	}
	expectedToken(parent->addChild(new Node), TERM);
	while(sym->tag==SP_COMMA) {
		expectedToken(parent->addChild(new Node), SP_COMMA);
		if (sym->tag==SP_L_TR_BRACKET)
			parseAttribute(parent->addChild(new Node));
		expectedToken(parent->addChild(new Node), TERM);
	}
	expectedToken(parent->addChild(new Node), SP_DOT);
}
//nonterm_decl = '%nonterm' [attr] @NONTERM { ',' [attr] @NONTERM }  '.' .
void Parser::parseNonTermDecl(Node *parent) {
	expectedToken(parent->addChild(new Node), SP_DECL_NONTERM);
	if (sym->tag==SP_L_TR_BRACKET)
		parseAttribute(parent->addChild(new Node));
	expectedToken(parent->addChild(new Node), SP_NON_TERM);
	while(sym->tag==SP_COMMA) {
		expectedToken(parent->addChild(new Node), SP_COMMA);
		if (sym->tag==SP_L_TR_BRACKET)
			parseAttribute(parent->addChild(new Node));
		expectedToken(parent->addChild(new Node), SP_NON_TERM);
	}
	expectedToken(parent->addChild(new Node), SP_DOT);
}
//attr = '<' ( TYPE ) '>' .
void Parser::parseAttribute(Node *parent) {
	expectedToken(parent->addChild(new Node), SP_L_TR_BRACKET);
	expectedToken(parent->addChild(new Node), SP_TYPE);
	expectedToken(parent->addChild(new Node), SP_R_TR_BRACKET);
}
//rule = @NONTERM [@SEMRULE]'=' multi_addendum '.' .
void Parser::parseRule(Node *parent) {
	printf("Rule...\n");
	expectedToken(parent->addChild(new Node), SP_NON_TERM);
	if (sym->tag==SP_SEM_RULE)
		expectedToken(parent->addChild(new Node), SP_SEM_RULE);
	expectedToken(parent->addChild(new Node), SP_EQUAL);
	parseMultiAddendum(parent->addChild(new Node));
	expectedToken(parent->addChild(new Node), SP_DOT);
}
//multi_addendum = addendum {'|' addendum}.
void Parser::parseMultiAddendum(Node *parent) {
	parseAddendum(parent->addChild(new Node));
	while (sym->tag==SP_DIR_SLASH) {
		expectedToken(parent->addChild(new Node), SP_DIR_SLASH);
		parseAddendum(parent->addChild(new Node));
	}
}
//addendum = factor [@SEMRULE] {factor [@SEMRULE] }.
void Parser::parseAddendum(Node *parent) {
	//printf("parseAddendum:%s,%d\n",Domains[sym->tag], num );
	parseFactor(parent->addChild(new Node));
	if (sym->tag==SP_SEM_RULE)
		expectedToken(parent->addChild(new Node), SP_SEM_RULE);
	while (sym->tag==SP_L_SQ_BRACKET || sym->tag==SP_L_BRACE || sym->tag==SP_L_PARENT || sym->tag==SP_NON_TERM || sym->tag==TERM) {
		parseFactor(parent->addChild(new Node));
		if (sym->tag==SP_SEM_RULE)
			expectedToken(parent->addChild(new Node), SP_SEM_RULE);
	}
}
//factor = '[' multi_addendum ']' |
//		  '{' multi_addendum '}' |
//		   '(' multi_addendum ')' |
//		    ident | .
void Parser::parseFactor(Node *parent) {
	//printf("parseFactor:%s,%d\n",Domains[sym->tag], num );
	switch (sym->tag) {
		case  SP_L_SQ_BRACKET:
			expectedToken(parent->addChild(new Node), SP_L_SQ_BRACKET);
			parseMultiAddendum(parent->addChild(new Node));
			expectedToken(parent->addChild(new Node), SP_R_SQ_BRACKET);
			break;
		case SP_L_BRACE:
			expectedToken(parent->addChild(new Node), SP_L_BRACE);
			parseMultiAddendum(parent->addChild(new Node));
			expectedToken(parent->addChild(new Node), SP_R_BRACE);
			break;
		case SP_L_PARENT:
			expectedToken(parent->addChild(new Node), SP_L_PARENT);
			parseMultiAddendum(parent->addChild(new Node));
			expectedToken(parent->addChild(new Node), SP_R_PARENT);
			break;
		case SP_NON_TERM:
			parseIdent(parent->addChild(new Node));
			break;
		case TERM:
			parseIdent(parent->addChild(new Node));
			break;
		//default:
			//printf("ErrorUnexpectedTokenInParseFactor:%s,%d\n", Domains[sym->tag], num );
	}
}
//ident = @TERM | @NONTERM .
void Parser::parseIdent(Node *parent) {
	switch (sym->tag) {
		case  TERM:
			expectedToken(parent->addChild(new Node), TERM);
			break;
		case SP_NON_TERM:
			expectedToken(parent->addChild(new Node), SP_NON_TERM);
			break;
		default:
			printf("ErrorUnexpectedTokenInParseIdent:%s,%d\n", Domains[sym->tag], num );
	}
}

YYSTYPE* Parser::nextToken() {
	return &tokens[++num];
}

void Parser::expectedToken(Node *parent, int tag) {
	if (sym->tag == tag) {
		parent->addChild(new Node(sym));
		sym = nextToken();
	} else {
		printf("ErrorUnexpectedTokenInExpectedToken:%s,%d|InsteadOf:%s\n", Domains[sym->tag], num, Domains[tag] );
	}
}
