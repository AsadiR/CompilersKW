

#ifndef NODE_H_
#define NODE_H_

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include "lexer/lexer.h"
extern char const* Domains[];
using namespace std;

class DeclElem {
public:
	YYSTYPE *sym;
	string attr;
	DeclElem(YYSTYPE* sym, YYSTYPE* type);
	virtual ~DeclElem();
	string to_string();
private:
	string stringValue;
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
	Factor(int mode, MultiAddendum *m_addendum);
	Factor(int mode, YYSTYPE *ident);
	Factor(int mode);
	~Factor();
	string to_string();
	int mode;
	YYSTYPE *semRule;
	MultiAddendum* m_addendum;
	YYSTYPE *ident;
private:
	string strValue;
};

class Addendum {
public:
	Addendum();
	~Addendum();
	string to_string();
	vector<Factor*> factors;
private:
	string strValue;
};

class MultiAddendum {
public:
	MultiAddendum();
	~MultiAddendum();
	string to_string();
	vector<Addendum*> addendums;
private:
	string strValue;
};

class Rule {
public:
	Rule(YYSTYPE *lpart, MultiAddendum *rpart, YYSTYPE *semRule);
	virtual ~Rule();
	YYSTYPE *lpart;
	MultiAddendum *rpart;
	YYSTYPE *semRule;
	string to_string();
private:
	string strValue;
};

class Node {
public:
	//дочерние узлы
	Node* parent;
	vector<Node*> children;
	YYSTYPE *token;
	//атрибут узла
	// ATTR attr;
	Node(Node *parent);
	Node* addChild(Node *child);

	//ищет вершины с токеном с тэгом tag,
	//используя обход в глубину из текущей вершины
	//добавляет их в вектор
	void findNodes(int tag, vector<Node*>* listOfNodes);

	//генерирует список объявленных терминалов и помещает их в tdv
	vector<DeclElem*>* getTermDeclVector(vector<DeclElem*>* tdv);
	vector<DeclElem*>* getNonTermDeclVector(vector<DeclElem*>* tdv);

	vector<Rule*>* getRules(vector<Rule*>* rv);

	Node(Node *parent, YYSTYPE *token);
	string to_string();
	virtual ~Node();
private:
	string stringValue;
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
	RuleFactory(Node *ruleNode);
	virtual ~RuleFactory();
	Rule* create();
	MultiAddendum* createMultiAddendum(Node* parent);
	Addendum* createAddendum(Node* parent);
	Factor* createFactor(Node *parent);
};


#endif /* NODE_H_ */
