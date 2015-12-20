#include "GeneratedParser.h"
Node* GeneratedParser::parse() {
	#line 1 "resources/inputGrammar"
	 
	#line 6 "results/GeneratedParser.cpp"
	root = new Node(NULL, "program");
	parse_program(root);
	if (sym->tag!=0)
		throw runtime_error("expected EOF!");
	#line 21 "resources/inputGrammar"
	 
	#line 13 "results/GeneratedParser.cpp"
	return root;
}
YYSTYPE* GeneratedParser::next_token() {
	if (num<tokens.size())
		return tokens[++num];
	else
		return tokens[num];
}
void GeneratedParser::expected_token(Node *parent, int tag) {
	if (sym->tag == tag) {
		sym = next_token();
	} else {
		throw runtime_error(
			"Unexpected_token:"
			+string(Domains[sym->tag])
			+" instead of: "
			+string(Domains[tag])
			+"\n");
	}
}
GeneratedParser::GeneratedParser(vector<YYSTYPE*> &t) {
	root = NULL;
	num = 0;
	this->tokens = t;
	sym = tokens[0];
}
GeneratedParser::~GeneratedParser() {}
void GeneratedParser::parse_program(Node *parent) {
	switch(sym->tag) {
		case PRINT:
			{
				parse_statement(parent->addChild(new Node(parent,"statement")));
				expected_token(parent->addChild(new Node(parent,sym)), SEMICOLON);
				parse_program(parent->addChild(new Node(parent,"program")));
				break;
			}
	}
}
void GeneratedParser::parse_statement(Node *parent) {
	expected_token(parent->addChild(new Node(parent,sym)), PRINT);
	parse_out(parent->addChild(new Node(parent,"out")));
	parse_outlist(parent->addChild(new Node(parent,"outlist")));
}
void GeneratedParser::parse_outlist(Node *parent) {
	switch(sym->tag) {
		case SP_COMMA:
			{
				expected_token(parent->addChild(new Node(parent,sym)), SP_COMMA);
				parse_out(parent->addChild(new Node(parent,"out")));
				parse_outlist(parent->addChild(new Node(parent,"outlist")));
				break;
			}
	}
}
void GeneratedParser::parse_out(Node *parent) {
	int value1 = parse_expr(parent->addChild(new Node(parent,"expr")));
	#line 8 "resources/inputGrammar"
	cout<<value1<<endl;
	#line 72 "results/GeneratedParser.cpp"
}
int GeneratedParser::parse_expr(Node *parent) {
	int result;
	int value1 = parse_addendum(parent->addChild(new Node(parent,"addendum")));
	#line 9 "resources/inputGrammar"
	result=value1;
	#line 79 "results/GeneratedParser.cpp"
	while ( sym->tag==PLUS || sym->tag==MINUS) {
		switch(sym->tag) {
			case PLUS:
				{
					expected_token(parent->addChild(new Node(parent,sym)), PLUS);
					int value2 = parse_addendum(parent->addChild(new Node(parent,"addendum")));
					#line 9 "resources/inputGrammar"
					result+=value2;
					#line 88 "results/GeneratedParser.cpp"
					break;
				}
			case MINUS:
				{
					expected_token(parent->addChild(new Node(parent,sym)), MINUS);
					int value2 = parse_addendum(parent->addChild(new Node(parent,"addendum")));
					#line 10 "resources/inputGrammar"
					result-=value2;
					#line 97 "results/GeneratedParser.cpp"
					break;
				}
			default:
				throw runtime_error(
					"Unexpected_tokenInParseIdent:"
					+string(Domains[sym->tag])
					+"\n");
		}
	}
	return result;
}
int GeneratedParser::parse_addendum(Node *parent) {
	int result;
	int value1 = parse_factor(parent->addChild(new Node(parent,"factor")));
	#line 11 "resources/inputGrammar"
	result=value1;
	#line 114 "results/GeneratedParser.cpp"
	while ( sym->tag==SP_MULTI || sym->tag==DIVIDE) {
		switch(sym->tag) {
			case SP_MULTI:
				{
					expected_token(parent->addChild(new Node(parent,sym)), SP_MULTI);
					int value2 = parse_factor(parent->addChild(new Node(parent,"factor")));
					#line 11 "resources/inputGrammar"
					result*=value2;
					#line 123 "results/GeneratedParser.cpp"
					break;
				}
			case DIVIDE:
				{
					expected_token(parent->addChild(new Node(parent,sym)), DIVIDE);
					int value2 = parse_factor(parent->addChild(new Node(parent,"factor")));
					#line 12 "resources/inputGrammar"
					result/=value2;
					#line 132 "results/GeneratedParser.cpp"
					break;
				}
			default:
				throw runtime_error(
					"Unexpected_tokenInParseIdent:"
					+string(Domains[sym->tag])
					+"\n");
		}
	}
	return result;
}
int GeneratedParser::parse_factor(Node *parent) {
	int result;
	#line 13 "resources/inputGrammar"
	 
	#line 148 "results/GeneratedParser.cpp"
	switch(sym->tag) {
		case SP_L_PARENT:
			{
				expected_token(parent->addChild(new Node(parent,sym)), SP_L_PARENT);
				int value1 = parse_expr(parent->addChild(new Node(parent,"expr")));
				expected_token(parent->addChild(new Node(parent,sym)), SP_R_PARENT);
				#line 13 "resources/inputGrammar"
				result=value1;
				#line 157 "results/GeneratedParser.cpp"
				break;
			}
		case NUMBER:
			{
				int value1 = *(sym->attr->__int__);
				expected_token(parent->addChild(new Node(parent,sym)), NUMBER);
				#line 14 "resources/inputGrammar"
				result=value1;
				#line 166 "results/GeneratedParser.cpp"
				break;
			}
		case MINUS:
			{
				expected_token(parent->addChild(new Node(parent,sym)), MINUS);
				int value1 = parse_factor(parent->addChild(new Node(parent,"factor")));
				#line 15 "resources/inputGrammar"
				result=-value1;
				#line 175 "results/GeneratedParser.cpp"
				break;
			}
		case DIVIDE:
			{
				expected_token(parent->addChild(new Node(parent,sym)), DIVIDE);
				int value1 = parse_factor(parent->addChild(new Node(parent,"factor")));
				expected_token(parent->addChild(new Node(parent,sym)), SP_COMMA);
				int value2 = parse_factor(parent->addChild(new Node(parent,"factor")));
				expected_token(parent->addChild(new Node(parent,sym)), SP_COMMA);
				int value3 = parse_factor(parent->addChild(new Node(parent,"factor")));
				expected_token(parent->addChild(new Node(parent,sym)), SP_COMMA);
				int value4 = parse_factor(parent->addChild(new Node(parent,"factor")));
				expected_token(parent->addChild(new Node(parent,sym)), DIVIDE);
				#line 18 "resources/inputGrammar"
				if (value1 < 0) result = value2; else if (value1 == 0) result = value3; else result = value4;
				#line 191 "results/GeneratedParser.cpp"
				break;
			}
		case SP_MULTI:
			{
				expected_token(parent->addChild(new Node(parent,sym)), SP_MULTI);
				int value1 = parse_factor(parent->addChild(new Node(parent,"factor")));
				#line 19 "resources/inputGrammar"
				result=0; int N=0;
				#line 200 "results/GeneratedParser.cpp"
				while ( sym->tag==SP_COMMA) {
					expected_token(parent->addChild(new Node(parent,sym)), SP_COMMA);
					int value2 = parse_factor(parent->addChild(new Node(parent,"factor")));
					#line 20 "resources/inputGrammar"
					N++; if (value1==N) result=value2;
					#line 206 "results/GeneratedParser.cpp"
				}
				expected_token(parent->addChild(new Node(parent,sym)), SP_MULTI);
				break;
			}
		default:
			throw runtime_error(
				"Unexpected_tokenInParseIdent:"
				+string(Domains[sym->tag])
				+"\n");
	}
	return result;
}
