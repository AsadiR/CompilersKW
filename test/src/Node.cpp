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
		throw runtime_error("More than one SP_DECL_TERM!\n");

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

vector<DeclElem*>* Node::getNonTermDeclVector(vector<DeclElem*>* tdv) {
	vector<Node*> *nodes = new vector<Node*>();
	findNodes(SP_DECL_TERM,nodes);
	if (nodes->size()==0)
		return tdv;
	if (nodes->size()>1)
		throw runtime_error("More than one SP_DECL_NONTERM!\n");

	cout << "SP_DECL_NONTERM is found\n";
	Node* term_decl_NT = (*nodes)[0]->parent;

	cout << "sizeOfDeclNonTermArray:" << term_decl_NT->children.size() << "\n";
	cout << term_decl_NT->to_string() <<"\n";

	DeclElemFactory factory(&(term_decl_NT->children), 1, SP_NON_TERM);
	while(factory.hasNext) {
		tdv->insert(tdv->end(), factory.create());
	}
	delete nodes;
	return tdv;
}

vector<Rule*>* Node::getRules(vector<Rule*>* rv) {
	vector<Node*> *nodes = new vector<Node*>();
	findNodes(SP_EQUAL,nodes);
	int i;
	for (i=0; i<(int)nodes->size(); i++) {
		Node* rule_NT = (*nodes)[i]->parent;
		RuleFactory factory(rule_NT);
		rv->insert(rv->end(), factory.create());
	}
	delete nodes;
	return rv;
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
		throw runtime_error("GenerationError! Factory is empty!\n");
	YYSTYPE *sym = NULL;
	YYSTYPE *type = NULL;

	if (array->at(index)->token==NULL) {
		cout << "checking of type ..." << "\n";
		vector<Node*> &attr = array->at(index)->children;
		if (attr.at(0)==NULL || attr.at(1)==NULL || attr.at(2)==NULL)
			throw runtime_error("GenerationError! Expected not null tokens!\n");
		if (attr.at(0)->token->tag!=SP_L_TR_BRACKET)
			throw runtime_error("GenerationError! Expected SP_L_TR_BRACKET!\n");
		if (attr.at(1)->token->tag!=SP_TYPE)
			throw runtime_error("GenerationError! Expected SP_TYPE!\n");
		type = attr.at(1)->token;
		if (attr.at(2)->token->tag!=SP_R_TR_BRACKET)
			throw runtime_error("GenerationError! Expected SP_R_TR_BRACKET!\n");
		index++;
	}

	cout << "checking of elem ..." << "\n";
	if (array->at(index)->token->tag!=elemTag)
		throw runtime_error("GenerationError! Expected specified elem!\n");
	sym = array->at(index)->token;
	index++;
	cout << "checking of delimiter ..." << "\n";
	if (array->at(index)->token->tag!=SP_COMMA && array->at(index)->token->tag!=SP_DOT)
		throw runtime_error("GenerationError! Expected SP_COMMA or SP_DOT!\n");
	if (array->at(index)->token->tag==SP_DOT) {
		if (index==(int)array->size()-1)
			hasNext = false;
		else
			throw runtime_error("GenerationError! Expected end of array after SP_DOT usage!\n");
	}
	index++;
	DeclElem* res = new DeclElem(sym,type);
	cout << "declElem is created" << "\n";
	return res;
}

RuleFactory::RuleFactory(Node *ruleNode) {
	this->ruleNode = ruleNode;
}

RuleFactory::~RuleFactory() {

}

Rule* RuleFactory::create() {
	cout << "try to create rule ..." << "\n";
	int i=0;
	if (ruleNode->children[i]->token->tag!=SP_NON_TERM)
		throw runtime_error("GenerationError! Expected SP_NON_TERM!\n");
	YYSTYPE *lpart = ruleNode->children[i]->token;
	i++;
	YYSTYPE *semR = NULL;
	if (ruleNode->children[i]->token->tag==SP_SEM_RULE) {
		semR = ruleNode->children[i]->token;
		i++;
	}
	i++;
	MultiAddendum* rpart = createMultiAddendum(ruleNode->children[i]);
	i++;
	if (ruleNode->children[i]->token->tag!=SP_DOT)
		throw runtime_error("GenerationError! Expected SP_DOT!\n");
	cout << "rule created" << "\n";
	return new Rule(lpart,rpart,semR);
}

MultiAddendum* RuleFactory::createMultiAddendum(Node* parent) {
	cout << "try to create multiAddendum ..." << "\n";
	MultiAddendum* md = new MultiAddendum();
	int i=0;
	Addendum *addendum;
	do {
		addendum = createAddendum(parent->children[i]);
		md->addendums.insert(md->addendums.end(), addendum);
		i++;
		if (i < (int)parent->children.size()
				&& parent->children[i]->token->tag!=SP_DIR_SLASH)
			throw runtime_error("GenerationError! Expected SP_DIR_SLASH!\n");
		i++;
	} while(i < (int)parent->children.size());
	cout << "multiAddendum created" << "\n";
	return md;
}

Addendum* RuleFactory::createAddendum(Node* parent) {
	cout << "try to create addendum ..." << "\n";
	int i=0;
	Addendum* a = new Addendum();
	do {
		Factor *factor = createFactor(parent->children[i]);
		i++;
		if (i < (int)parent->children.size()
				&& parent->children[i]->token!= NULL
				&& parent->children[i]->token->tag==SP_SEM_RULE) {
			factor->semRule = ruleNode->children[i]->token;
			i++;
		}
		a->factors.insert(a->factors.end(), factor);

	} while (i < (int)parent->children.size());
	cout << "addendum created" << "\n";
	return a;
}

Factor* RuleFactory::createFactor(Node* parent) {
	cout << "try to create factor ..." << "\n";
	Factor * factor = NULL;
	if (parent->children.size()==0)
		factor = new Factor(0);
	if (parent->children.size()==1) {
		YYSTYPE *ident = parent->children[0]->children[0]->token;
		factor = new Factor(1, ident);
	}
	if (parent->children.size()==3 && parent->children[0]->token->tag == SP_L_PARENT) {
		MultiAddendum *md = createMultiAddendum(parent->children[1]);
		factor = new Factor(2, md);
	}
	if (parent->children.size()==3 && parent->children[0]->token->tag == SP_L_SQ_BRACKET) {
		MultiAddendum *md = createMultiAddendum(parent->children[1]);
		factor = new Factor(3, md);
	}
	if (parent->children.size()==3 && parent->children[0]->token->tag == SP_L_BRACE) {
		MultiAddendum *md = createMultiAddendum(parent->children[1]);
		factor =  new Factor(4, md);
	}
	cout << "factor created" << "\n";
	return factor;
}


Factor::Factor(int mode, MultiAddendum *m_addendum) {
	this->mode = mode;
	this->m_addendum = m_addendum;
	this->semRule = NULL;
	this->ident = NULL;
}

Factor::Factor(int mode, YYSTYPE *ident) {
	this->mode = mode;
	this->m_addendum = m_addendum;
	this->semRule = NULL;
	this->ident = ident;
}

Factor::Factor(int mode) {
	this->mode = mode;
	this->m_addendum = NULL;
	this->semRule = NULL;
	this->ident = NULL;
}

Factor::~Factor() {
	if (m_addendum!=NULL)
		delete m_addendum;
}

string Factor::to_string() {
	if (mode==0)
		strValue = "";
	if (mode==1)
		strValue = string(Domains[ident->tag]);
	if (mode==2)
		strValue = "("+m_addendum->to_string()+")";
	if (mode==3)
		strValue = "["+m_addendum->to_string()+"]";
	if (mode==4)
		strValue = "{"+m_addendum->to_string()+"}";
	return strValue;

}

Addendum::Addendum() {

}
Addendum::~Addendum() {
	for (int i=0; i<(int)factors.size(); i++)
		delete factors[i];
}
string Addendum::to_string() {
	strValue = "";
	for (int i=0; i<(int)factors.size(); i++)
		strValue += " "+factors[i]->to_string();
	return strValue;
}

MultiAddendum::MultiAddendum() {

}

MultiAddendum::~MultiAddendum( ) {
	for (int i=0; i<(int)addendums.size(); i++)
		delete addendums[i];
}

string MultiAddendum::to_string() {
	strValue = "";
	for (int i=0; i<(int)addendums.size(); i++)
		strValue += (i==0?"":"|") + addendums[i]->to_string();
	return strValue;
}

Rule::Rule(YYSTYPE *lpart, MultiAddendum *rpart, YYSTYPE *semRule) {
	this->rpart = rpart;
	this->lpart = lpart;
	this->semRule = semRule;
}

Rule::~Rule() {
	if (rpart!=NULL)
		delete rpart;
}

string Rule::to_string() {
	strValue = "";
	strValue += string(Domains[lpart->tag]);
	strValue += " =";
	strValue += rpart->to_string();
	strValue += ".\n";
	return strValue;
}





