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
#include <list>
#include "lexer.h"
#include "Grammar.h"
#include "Parser.h"
#include "GeneratedParser.h"
#include <cctype>
#include <algorithm>
#include <typeinfo>
#define COMMON_PARSER 0
#define GENERATED_PARSER 1

using namespace std;

extern char const* Domains[];

//семантические правила могут храниться в классах: Grammar, Rule, Factor.
//Класс ManagerOfVariables позволяет связывать
//шифры сем. атрибутов с переменными, объявленными внутри функций.
class ManagerOfVariables {
public:
	list<int> layers;
	ManagerOfVariables();
	virtual ~ManagerOfVariables();
	string getNameOfNextVar();
	string getNameOfVar(string cipher);//имя по шифру переменной
	void beginNewLayer();
	void endLayer();
	void saveState();
	void restoreState();
private:
	unsigned int saved_num;
	unsigned int saved_layers_size;
	unsigned int num;
	string mainPartOfVarName;

};


class Generator {
public:
	Generator(string inputGrammarFileName,
			string logsFileName, int numOfParser);
	virtual ~Generator();
	void generateCppParser(string className, string pathToFile);
private:
	void writeSignature(string shift, Rule *r);
	void writeRule(Rule *r);
	void writeMultiAddendum(string shift, MultiAddendum *ma, ManagerOfVariables &manager);
	void writeAddendum(string shift, Addendum *a, ManagerOfVariables &manager);
	void writeFactor(string shift, Factor *f, ManagerOfVariables &manager);
	void writeSemRule(string shift, YYSTYPE *semRule, ManagerOfVariables &manager);
	void lexicalAnalysis();
	string getRetType(Rule *r);
	ofstream *fileH;
	ofstream *fileCpp;
	string className;
	vector<YYSTYPE*>  tokens;
	vector<YYLTYPE*> coords;
	char* inputGrammar;
	Grammar *grammar;
	ofstream *logs=NULL;
	Node *root;
	yyscan_t scanner;
	map<YYSTYPE*, YYLTYPE*> tokenToCoord;
	string inputGrammarFileName;
	string cppFileName;
	int line_counter_for_cpp;
	string nextLineCpp();
};

#endif /* SRC_GENERATOR_H_ */
