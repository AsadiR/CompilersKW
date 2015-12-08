/*
 * Grammar.h
 *
 *  Created on: 10 нояб. 2015 г.
 *      Author: ugin
 */

#ifndef GRAMMAR_H_
#define GRAMMAR_H_
#include <vector>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "lexer/lexer.h"
#include "Node.h"
extern char const* Domains[];
using namespace std;
#define EMPTY_RULE_TAG -1
#define MODE_EMPTY  0
#define MODE_SIMPLE  1
#define MODE_PARENTS  2
#define MODE_OPTIONAL  3
#define MODE_ITERATION  4

class Grammar;

class DeclElem {
public:
	YYSTYPE *sym;
	YYSTYPE *attrTypeToken;
	DeclElem(YYSTYPE* sym, YYSTYPE* type);
	virtual ~DeclElem();
	string to_string();
private:
	set<int> firstSet;
};

class MultiAddendum;

class Factor {
public:
	//виды режимов:
	//0 - пустой
	//1 - просто значение
	//2 - выражение в круглых скобках
	//3 - выражение в квадратных скобках
	//4 - выражение в фигурных скобках
	Factor(int mode, MultiAddendum *m_addendum, Grammar *grammar);
	Factor(int mode, YYSTYPE *ident, Grammar *grammar);
	Factor(int mode, Grammar *grammar);
	~Factor();
	string to_string();
	bool getFirst();
	set<int> firstSet;
	int mode;
	YYSTYPE *semRule;
	MultiAddendum* m_addendum;
	YYSTYPE *ident;
	Grammar *grammar;
};

class Addendum {
public:
	Addendum();
	~Addendum();
	string to_string();
	bool getFirst();
	set<int> firstSet;
	vector<Factor*> factors;
};

class MultiAddendum {
public:
	MultiAddendum();
	~MultiAddendum();
	string to_string();
	bool getFirst();
	set<int> firstSet;
	vector<Addendum*> addendums;
};

class Rule {
public:
	Rule(YYSTYPE *lpart, MultiAddendum *rpart, YYSTYPE *semRule);
	virtual ~Rule();
	YYSTYPE *lpart;
	MultiAddendum *rpart;
	YYSTYPE *semRule;
	string to_string();
	bool getFirst();
	set<int> firstSet;
};


class DeclElemFactory {
public:
	bool hasNext;
	DeclElem* create();
	DeclElemFactory(vector<Node*> *array, int index, int elemTag);
	virtual ~DeclElemFactory();
private:
	vector<Node*> *array;
	int index;
	//TERM or SP_NON_TERM
	int elemTag;
};

class RuleFactory {
public:
	Node *ruleNode;
	Grammar *grammar;
	RuleFactory(Node *ruleNode, Grammar *grammar);
	virtual ~RuleFactory();
	Rule* create();
	MultiAddendum* createMultiAddendum(Node* parent);
	Addendum* createAddendum(Node* parent);
	Factor* createFactor(Node *parent);
};

class Grammar {
public:
	Grammar(Node *root);
	virtual ~Grammar();
	string to_string();
	map<string,int>* strToInt;
	vector<Rule*>* rules;
	map<string,DeclElem*>* termDecl;
	map<string,DeclElem*>* nonTermDecl;
	YYSTYPE *initSemRule;
	YYSTYPE *finalSemRule;
private:
	vector<Rule*>* getRules(vector<Rule*>* rv);
	map<string,DeclElem*>* getNonTermDeclMap(map<string,DeclElem*>* tdm);
	map<string,DeclElem*>* getTermDeclMap(map<string,DeclElem*>* tdm);
	map<string,int>* boundNonTerms(map<string,int> *strToInt);
	void getFirst();
	Node *root;
	stringstream strValue;
};

#endif /* GRAMMAR_H_ */
