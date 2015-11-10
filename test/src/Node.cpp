/*
 * Node.cpp
 *
 *  Created on: 04.10.2015
 *      Author: ugin
 */

#include "Node.h"


Node::Node(Node *parent) {
	this->parent = parent;
	token = NULL;
}

Node::Node(Node *parent, YYSTYPE *token) {
	this->token = token;
	this->parent = parent;
}

Node* Node::addChild(Node *child) {
	children.insert(children.end(), child);
	return child;
}

string& Node::to_string() {
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
			stringValue += name + ", ";
		}
	}
	return stringValue;
}

void Node::findNodes(int tag, vector<Node*>* listOfNodes) {
	int i;
	if (this->token != NULL) {
		if (this->token->tag == tag)
			listOfNodes->insert(listOfNodes->end(), this);
		return;
	}

	for (i=0; i<(int)children.size(); i++) {
		children[i]->findNodes(tag,listOfNodes);
	}
	return;
}


Node::~Node() {
	for (int i=0; i<(int)children.size(); i++) {
		delete children[i];
	}
}





