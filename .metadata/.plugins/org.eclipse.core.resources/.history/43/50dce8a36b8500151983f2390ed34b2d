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

vector<DeclElem*>* Node::getTermDeclVector(vector<DeclElem*>* tdv) {
	vector<Node*> *nodes = new vector<Node*>();
	findNodes(SP_DECL_TERM,nodes);
	if (nodes->size()==0)
		return tdv;
	if (nodes->size()>1)
		throw new runtime_error("More than one SP_DECL_TERM!\n");

	cout << "SP_DECL_TERM is found\n";
	Node* term_decl_NT = (*nodes)[0]->parent;

	cout << "sizeOfDeclTermArray:" << term_decl_NT->children.size() << "\n";
	cout << term_decl_NT->to_string() <<"\n";

	DeclElemFactory factory(&(term_decl_NT->children), 1, TERM);
	while(factory.hasNext) {
		tdv->insert(tdv->end(), factory.create());
	}
	delete nodes;
	return tdv;
}





Node::~Node() {
	for (int i=0; i<(int)children.size(); i++) {
		delete children[i];
	}
}

DeclElem::DeclElem(YYSTYPE* sym, YYSTYPE* type) {
	this->sym = sym;
	if (type != NULL ) {
		this->attr = string(type->value);
	}
}

DeclElem::~DeclElem() {

}

string DeclElem::to_string() {
	stringValue = "";
	stringValue += "<"+attr+">"+string(Domains[sym->tag]);
	return stringValue;
}

DeclElemFactory::DeclElemFactory(vector<Node*> *array, int index, int elemTag) {
	this->array = array;
	this->index = index;
	this->elemTag = elemTag;
	this->hasNext = true;
}

DeclElemFactory::~DeclElemFactory() {

}

DeclElem* DeclElemFactory::create() {
	cout << "try to create declElem ..." << "\n";
	if (hasNext==false)
		throw new runtime_error("GenerationError! Factory is empty!\n");
	YYSTYPE *sym = NULL;
	YYSTYPE *type = NULL;

	if (array->at(index)->token==NULL) {
		cout << "checking of type ..." << "\n";
		vector<Node*> &attr = array->at(index)->children;

		if (attr.at(0)==NULL || attr.at(1)==NULL || attr.at(2)==NULL)
			throw new runtime_error("GenerationError! Expected not null tokens!\n");
		if (attr.at(0)->token->tag!=SP_L_TR_BRACKET)
			throw new runtime_error("GenerationError! Expected SP_L_TR_BRACKET!\n");
		if (attr.at(1)->token->tag!=SP_TYPE)
			throw new runtime_error("GenerationError! Expected SP_TYPE!\n");
		type = attr.at(1)->token;
		if (attr.at(2)->token->tag!=SP_R_TR_BRACKET)
			throw new runtime_error("GenerationError! Expected SP_R_TR_BRACKET!\n");
		index++;
	}
	cout << "checking of elem ..." << "\n";
	if (array->at(index)->token->tag!=elemTag)
		throw new runtime_error("GenerationError! Expected specified elem!\n");
	sym = array->at(index)->token;
	index++;
	cout << "checking of delimiter ..." << "\n";
	if (array->at(index)->token->tag!=SP_COMMA && array->at(index)->token->tag!=SP_DOT)
		throw new runtime_error("GenerationError! Expected SP_COMMA or SP_DOT!\n");
	if (array->at(index)->token->tag==SP_DOT) {
		if (index==(int)array->size()-1)
			hasNext = false;
		else
			throw new runtime_error("GenerationError! Expected end of array after SP_DOT usage!\n");
	}
	index++;
	DeclElem* res = new DeclElem(sym,type);
	cout << "declElem is created" << "\n";
	return res;
}




