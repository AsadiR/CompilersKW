/*
 * Node.cpp
 *
 *  Created on: 04.10.2015
 *      Author: ugin
 */

#include "Node.h"


Node::Node() {
	token = NULL;
}

Node::Node(YYSTYPE *token) {
	this->token = token;
	// TODO Auto-generated constructor stub
}

Node* Node::addChild(Node *child) {
	children.insert(children.end(), child);
	return child;
}

string Node::to_string() {
	string res = "";
	for (int i=0; i<(int)children.size(); i++) {
		if(children[i]->token==NULL){
			res += children[i]->to_string();
		} else {
			string name(Domains[children[i]->token->tag]);
			res += name + ", ";
		}
	}
	return res;
}

Node::~Node() {
	for (int i=0; i<(int)children.size(); i++) {
		delete children[i];
	}
}



