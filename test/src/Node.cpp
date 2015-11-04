/*
 * Node.cpp
 *
 *  Created on: 04.10.2015
 *      Author: ugin
 */

#include "Node.h"

Node::Node() {
	// TODO Auto-generated constructor stub
}

Node::Node(YYSTYPE *token) {
	this->token = token;
	// TODO Auto-generated constructor stub
}

Node* Node::addChild(Node *child) {
	children.insert(children.end(), child);
	return child;
}

Node::~Node() {
	for (int i=0; i<children.size(); i++) {
		delete children[i];
	}
}


