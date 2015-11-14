/*
 * Parser.h
 *
 *  Created on: 11.10.2015
 *      Author: ugin
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "lexer/lexer.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include "Node.h"

using namespace std;

class Parser {
private:
	Node* root;
	unsigned int num;
	vector<YYSTYPE*> tokens;
	YYSTYPE *sym;
	YYSTYPE* nextToken();
	void parseGrammar(Node *parent);
	void parseInit(Node *parent);
	void parseTermDecl(Node *parent);
	void parseNonTermDecl(Node *parent);
	void parseAttribute(Node *parent);
	void parseRule(Node *parent);
	void parseMultiAddendum(Node *parent);
	void parseAddendum(Node *parent);
	void parseFactor(Node *parent);
	void parseIdent(Node *parent);
	void expectedToken(Node *parent, int tag);

public:
	Node* parse();
	Parser(vector<YYSTYPE*> t);
	virtual ~Parser();
};

#endif /* PARSER_H_ */
