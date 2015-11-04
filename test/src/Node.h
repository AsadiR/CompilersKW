

#ifndef NODE_H_
#define NODE_H_

#include <vector>
#include<string>
#include "lexer/lexer.h"
using namespace std;

extern char const* Domains[];


class Node {
public:
	//дочерние узлы
	vector<Node*> children;
	YYSTYPE *token;
	//атрибут узла
	// ATTR attr;
	Node();
	Node* addChild(Node *child);
	Node(YYSTYPE *token);
	string to_string();
	virtual ~Node();
};

#endif /* NODE_H_ */
