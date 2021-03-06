#ifndef GENERATEDPARSER_H_
#define GENERATEDPARSER_H_
#include "lexer/lexer.h"
#include "Node.h"
#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;
class GeneratedParser {
private:
	Node *root;
	unsigned int num;
	vector<YYSTYPE*> tokens;
	YYSTYPE *sym;
	YYSTYPE* next_token();
	void parse_program(Node *parent);
	void parse_statement(Node *parent);
	void parse_outlist(Node *parent);
	void parse_out(Node *parent);
	int parse_expr(Node *parent);
	int parse_addendum(Node *parent);
	int parse_factor(Node *parent);
	void expected_token(Node *parent, int tag);
public:
	Node* parse();
	GeneratedParser(vector<YYSTYPE*> &t);
	virtual ~GeneratedParser();
};
#endif
