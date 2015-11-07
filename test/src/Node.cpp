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
	stringValue = "";
	if (this->token!=NULL) {
		stringValue += string(Domains[this->token->tag]);
		return stringValue;
	}

	for (int i=0; i<(int)children.size(); i++) {
		if(children[i]->token==NULL){
			stringValue += children[i]->to_string();
		} else {
			string name(Domains[children[i]->token->tag]);
			stringValue += name;
			cout << name;
			if ( i!=(int)children.size()-1 ) stringValue += ", ";
		}
	}
	return stringValue;
}

Node* Node::findNode(int tag) {
	int i;
	if (this->token != NULL) {
		if (this->token->tag == tag) return this;
		return NULL;
	}
	Node *cur = NULL;
	for (i=0; i<(int)children.size() && cur==NULL; i++) {
		cur = children[i]->findNode(tag);
	}
	return cur;
}

Node::~Node() {
	for (int i=0; i<(int)children.size(); i++) {
		delete children[i];
	}
}



