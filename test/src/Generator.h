/*
 * Generator.h
 *
 *  Created on: 14 нояб. 2015 г.
 *      Author: ugin
 */

#ifndef SRC_GENERATOR_H_
#define SRC_GENERATOR_H_
#include <string>
#include <fstream>
#include <vector>
#include "lexer/lexer.h"
#include "Grammar.h"
#include "Parser.h"
#include <cctype>
#include <algorithm>
#include <typeinfo>
using namespace std;

extern char const* Domains[];

class Generator {
public:
	Generator(string inputGrammarFileName,
			string logsFileName);
	virtual ~Generator();
	void generateCppParser(string parserClassName);
private:
	void writeSignature(string shift, Rule *r);
	void writeRule(int shift);
	void writeMultiAddendum(int shift);
	void writeAddendum(int shift);
	void writeFactor(int shift);
	void lexicalAnalysis();
	ofstream *fileH;
	ofstream *fileCpp;

	vector<YYSTYPE*>  tokens;
	vector<YYLTYPE*> coords;
	char* inputGrammar;
	Grammar *grammar;
	ofstream *logs=NULL;
	Node *root;
	yyscan_t scanner;
};

#endif /* SRC_GENERATOR_H_ */
