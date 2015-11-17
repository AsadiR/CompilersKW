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
#include "lexer.h"
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
	void writeRule(Rule *r);
	void writeMultiAddendum(string shift, MultiAddendum *ma);
	void writeAddendum(string shift, Addendum *a);
	void writeFactor(string shift, Factor *f);
	void lexicalAnalysis();
	string getRetType(Rule *r);
	ofstream *fileH;
	ofstream *fileCpp;
	string parserClassName;
	vector<YYSTYPE*>  tokens;
	vector<YYLTYPE*> coords;
	char* inputGrammar;
	Grammar *grammar;
	ofstream *logs=NULL;
	Node *root;
	yyscan_t scanner;
};

#endif /* SRC_GENERATOR_H_ */
