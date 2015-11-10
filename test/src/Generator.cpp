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
	//const char* fname = "grammar.txt";
	if ((fp=fopen(fname, "r+"))==NULL) {
		printf("Cannot open file.");
		return 1;
	}
	char buf[1024];
	int i = 0;
	for (i = 0; i<1024; i++) {
		buf[i] = 0;
	}

	fread(buf, 1024*sizeof(char), 1024*sizeof(char), fp);

	printf("%s\n", buf);

	int tag ;
	YYSTYPE value;
	YYLTYPE coords ;
	yyscan_t scanner;

	vector<YYSTYPE> tokens;

	struct Extra extra;
	printf("Lexer...\n");
	init_scanner(buf, &scanner, &extra);
	do
	{
		tag = yylex (&value, &coords, scanner );
		if ( tag != 0) {
			 printf("tag:%d,name:%s \n", tag, Domains[tag]);
			 printf("value:%s\n", value.value);
			 tokens.insert(tokens.end(), value);
		}
	} while ( tag != 0);
	printf("Parser...\n");
	Parser p(tokens);
	Node *root = p.parse();


	Grammar gr(root);
	cout << gr.to_string();

	for (std::vector<YYSTYPE>::iterator it = tokens.begin() ; it != tokens.end(); ++it)
		free((*it).value);
	delete root;
	destroy_scanner(scanner);
	cout << "\nEND\n";
	return 0;
}
