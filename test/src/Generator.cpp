#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "lexer/lexer.h"
#include "Parser.h"
#include <iostream>
#include "Grammar.h"

using namespace std;

extern char const* Domains[];

int main ()	{
	FILE *fp;
	const char* fname = "firstGrammar.txt";
	if ((fp=fopen(fname, "r+"))==NULL) {
		printf("Cannot open file.");
		return 1;
	}

	fseek(fp , 0 , SEEK_END);
	long lSize = ftell(fp);
	rewind(fp);

	char *buf = (char*)malloc(lSize*sizeof(char)+1);
	unsigned int i = 0;
	for (i=0; i<lSize+1; i++) {
		buf[i] = 0;
	}

	fread(buf, 1, lSize, fp);

	printf("%s\n", buf);

	int tag ;
	YYLTYPE coords ;
	yyscan_t scanner;

	vector<YYSTYPE*> tokens;

	struct Extra extra;
	printf("Lexer...\n");
	init_scanner(buf, &scanner, &extra);
	do
	{
		YYSTYPE *value = new YYSTYPE;
		tag = yylex (value, &coords, scanner);
		printf("tag:%d,name:%s \n", tag, Domains[tag]);
		printf("value:%s\n", value->value);
		tokens.insert(tokens.end(), value);
	} while ( tag != 0);
	printf("Parser...\n");
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
		if ((*it)->value!=NULL) free((*it)->value);
		delete(*it);
	}
	delete root;
	free(buf);
	destroy_scanner(scanner);
	cout << "\nEND\n";
	return 0;
}
