#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "lexer/lexer.h"
#include "Parser.h"
#include <iostream>
#include "Grammar.h"
#include <typeinfo>
#include <fstream>

using namespace std;

extern char const* Domains[];

int main ()	{
	unsigned int i;
	const char* filename = "inputGrammar.txt";
	std::ifstream in(filename);
	std::string contents((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	char *buf = (char*)contents.c_str();

	cout << string(buf);

	int tag ;
	YYLTYPE coords ;
	yyscan_t scanner;

	vector<YYSTYPE*> tokens;

	struct Extra extra;
	cout << "Lexer...\n";
	init_scanner(buf, &scanner, &extra);
	do
	{
		YYSTYPE *value = new YYSTYPE;
		tag = yylex (value, &coords, scanner);
		printf("tag:%d,name:%s \n", tag, Domains[tag]);
		if (value->attr!=NULL && typeid(*(value->attr->__string__))== typeid(string))
			cout << "attr: " << *(value->attr->__string__) <<"\n";
		tokens.insert(tokens.end(), value);
	} while ( tag != 0);
	cout << "Parser...\n";
	Parser p(tokens);
	Node *root = p.parse();

	Grammar gr(root);
	cout << gr.to_string();

	cout << "first set test:" << "\n";
	for (i=0;i<gr.rules->size(); i++) {
		Rule *r = gr.rules->at(i);
		set<int> &s = r->firstSet;
		for (std::set<int>::iterator it = s.begin() ; it != s.end(); ++it) {
			cout << (*it!=-1 ? string(Domains[*it])+" " : "empty ");
		}
		cout  << "\n";
	}


	for (vector<YYSTYPE*>::iterator it = tokens.begin() ; it != tokens.end(); ++it) {
		if ((*it)->attr!=NULL) {
			TYPES *t = (*it)->attr;
			if (t->__string__!=NULL) delete t->__string__;
			delete t;
		}
		delete(*it);
	}
	delete root;
	destroy_scanner(scanner);
	cout << "\nEND\n";
	return 0;
}
