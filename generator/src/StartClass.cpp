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
#include "Generator.h"
#include "GeneratedParser.h"

using namespace std;

void lexicalAnalysis(vector<YYSTYPE*> &tokens, vector<YYLTYPE*> &coords, char *buf, yyscan_t &scanner) {
	int tag ;
	struct Extra extra;
	init_scanner(buf, &scanner, &extra);

	do
	{
		YYSTYPE *value = new YYSTYPE;
		YYLTYPE *coord = new YYLTYPE;
		tag = yylex (value, coord, scanner);
		tokens.insert(tokens.end(), value);
		coords.insert(coords.end(), coord);
	} while ( tag != 0);
}

void freeLaMemory(vector<YYSTYPE*> &tokens, vector<YYLTYPE*> &coords, yyscan_t &scanner) {
	for (vector<YYSTYPE*>::iterator it = tokens.begin() ; it != tokens.end(); ++it) {
			/*cout << "check" << endl;
			cout << (*it)->attr << endl;
			cout << (*it)->attr_type << endl;*/
			if ((*it)->attr!=NULL) {
				TYPES *t = (*it)->attr;
				if (t->__int__!=NULL) delete (t->__int__);
				delete t;
			}
			if ((*it)->attr_type!=NULL) {
				delete (*it)->attr_type;
			}
			delete(*it);
	}

	for (vector<YYLTYPE*>::iterator it = coords.begin() ; it != coords.end(); ++it)
			delete(*it);
	destroy_scanner(scanner);
}



void test0() {
	Generator *generator = new Generator("resources/firstGrammar.txt",
			"results/logs.txt", COMMON_PARSER);
	generator->generateCppParser("GeneratedParser", "results/GeneratedParser");
	delete generator;
}

void test1() {
	Generator *generator = new Generator("resources/firstGrammar.txt",
			"results/logs.txt", GENERATED_PARSER);
	generator->generateCppParser("GeneratedParser", "results/GeneratedParser");
	delete generator;
}

void test2() {
	Generator *generator = new Generator("resources/inputGrammar.txt",
			"results/logs.txt", GENERATED_PARSER);
	generator->generateCppParser("GeneratedParser", "results/GeneratedParser");
	delete generator;
}

void test3() {
	std::ifstream in("resources/test.txt");
	std::string context((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	//string context = "PRINT 1+1, 3*2+1; PRINT -(7-2/2);";
	yyscan_t scanner;
	vector<YYSTYPE*> tokens;
	vector<YYLTYPE*> coords;
	char *buf = (char*)context.c_str();
	lexicalAnalysis(tokens, coords, buf, scanner);

	GeneratedParser parser(tokens);
	Node *root = parser.parse();

	cout << (root->to_string());
	freeLaMemory(tokens, coords, scanner);
	delete root;
}

int main ()	{
	int mode;
	cin >> mode;
	switch (mode) {
		case 0:
			test0();
			break;
		case 1:
			test1();
			break;
		case 2:
			test2();
			break;
		case 3:
			test3();
			break;
	}

	cout << "\nEND_OF_PROGRAM\n";
	return 0;
}

/*unsigned int i;
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
destroy_scanner(scanner);*/
