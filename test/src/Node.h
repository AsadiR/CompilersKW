

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


#endif /* NODE_H_ */
