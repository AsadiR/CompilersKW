

#ifndef NODE_H_
#define NODE_H_

#include <vector>
#include "lexer/lexer.h"
using namespace std;


class Node {
public:
	//дочерние узлы
	vector<Node*> children;
	YYSTYPE *token;
	//атрибут узла
	// ATTR attr;
	Node* addChild(Node *child);
	Node(YYSTYPE *token);
	Node();
	virtual ~Node();
};

#endif /* NODE_H_ */