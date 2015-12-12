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

void generate(string pathToInputGrammar, string className, string pathToFile, int mode) {
	Generator *generator = new Generator(pathToInputGrammar,
			"results/logs.txt", mode);
	generator->generateCppParser(className, pathToFile);
	delete generator;
}

void test(string testFileName) {
	std::ifstream in(testFileName.c_str());
	std::string context((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	yyscan_t scanner;
	vector<YYSTYPE*> tokens;
	vector<YYLTYPE*> coords;
	char *buf = (char*)context.c_str();
	lexicalAnalysis(tokens, coords, buf, scanner);

	GeneratedParser parser(tokens);
	Node *root = parser.parse();

	ofstream logs("results/testTree.txt", std::ofstream::out);
	logs << (root->to_string());
	freeLaMemory(tokens, coords, scanner);
	delete root;
}

int main ()	{
	char mode;
	cout << "Enter mode-flag:"
			<< "\n";
	cin >> mode;
	if (mode!='t' && mode!='g' && mode!='s') {
		cout << "The flag is not correct!" << "\n";
		cout << "Enter 'g' to generate parser" << "\n";
		cout << "Enter 't' to run test" << "\n";
		cout << "Enter 's' to self-aplicability test" << "\n";
		return 0;
	}
	switch (mode) {
	case 't':
		{
			string testFileName;
			cout << "Enter path to file with test sentence:"
					<< "\n";
			cin >> testFileName;
			test(testFileName);
			break;
		}
	case 'g':
		{
			string pathToInputGrammar;
			string className;
			string pathToFile;

			cout << "Enter path to file with grammar " << "\n";
			cin >> pathToInputGrammar;

			cout << "Enter name of parse-class:"
					<<"\n";
			cin >> className;

			cout << "Enter path to file:" <<"\n";
			cin >> pathToFile;

			generate(pathToInputGrammar, className, pathToFile, COMMON_PARSER);
			break;

		}

	case 's':
		{
			string pathToInputGrammar;
			string className;
			string pathToFile;
			pathToInputGrammar = "resources/firstGrammar";
			className = "SecondParser";
			pathToFile = "results/SecondParser";
			generate(pathToInputGrammar, className, pathToFile, GENERATED_PARSER);
			break;
		}
	}

	cout << "\nThe program is completed successfully!\n";
	return 0;
}


/*
#(1)
#For self-applicability test:
#compile and run:
g
resources/firstGrammar
GeneratedParser
results/GeneratedParser

#compile and run:
s
#Compare GeneratedParser.cpp and SecondParser.cpp

#(2)
#For test of semantic rules:
#compile and run:
g
resources/inputGrammar
GeneratedParser
results/GeneratedParser
#compile and run:
t
resources/test

#compare results in console and analyze it
#also you can find parse-tree in log.txt file

 */
