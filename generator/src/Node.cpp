/*
 * Node.cpp
 *
 *  Created on: 04.10.2015
 *      Author: ugin
 */

#include "Node.h"


Node::Node(Node *parent, string nonTermName) {
	this->parent = parent;
	this->nonTermName = nonTermName;
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


void Node::buildStr(Node *parent, stringstream *res, string ident) {
	bool isItRoot = (parent == NULL);
	unsigned int l = (isItRoot? 0 : parent->children.size());
	bool isItLast = (l!=0 && (parent->children.at(l-1)==this));


	if (parent == NULL) {
		(*res) << (token==NULL ? nonTermName : Domains[token->tag]) << "\n";
	} else {
		(*res) << ident << (isItLast ? "└╴" : "├╴")
				<< (token==NULL ? nonTermName : Domains[token->tag]) << "\n";
		if (isItLast && this->children.empty())
			(*res) << ident << "\n";
		ident += string(isItLast ? "  " : "│ ");
	}
	for (unsigned int i = 0; i<children.size(); i++)
		this->children[i]->buildStr(this, res, ident);
}

string Node::to_string() {
	stringstream res;
	string ident;
	buildStr(NULL, &res, ident);
	return res.str();
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





