

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
	//0 - просто значение
	//0 - выражение в круглых скобках
	//2 - выражение в квадратных скобках
	//3 - выражение в фигурных скобках
	Factor(int mode);
	~Factor();
	int mode;
	vector<MultiAddendum*> m_addendums;
};

class Addendum {
public:
	Addendum();
	~Addendum();
	vector<Factor*> factors;
};

class MultiAddendum {
public:
	MultiAddendum();
	~MultiAddendum();
	vector<Addendum*> addendums;
};

class Rule {
public:
	Rule(YYSTYPE *rpart, MultiAddendum *lpart);
	virtual ~Rule();
	YYSTYPE *rpart;
	MultiAddendum *lpart;
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
	RuleFactory();
	virtual ~RuleFactory();
	Rule* create();
};


#endif /* NODE_H_ */
