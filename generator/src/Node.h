

#ifndef NODE_H_
#define NODE_H_

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include "lexer/lexer.h"
#include <set>
#include <sstream>
extern char const* Domains[];
using namespace std;

class Node {
public:
	//дочерние узлы
	Node* parent;
	vector<Node*> children;
	string nonTermName;
	YYSTYPE *token;
	//атрибут узла
	// ATTR attr;
	Node(Node *parent, string nonTermName);
	Node* addChild(Node *child);

	//ищет вершины с токеном с тэгом tag,
	//используя обход в глубину из текущей вершины
	//добавляет их в вектор
	void findNodes(int tag, vector<Node*>* listOfNodes);

	Node(Node *parent, YYSTYPE *token);
	string to_string();
	virtual ~Node();
private:
	void buildStr(Node *parent, stringstream *res, string ident);
};

#endif /* NODE_H_ */
