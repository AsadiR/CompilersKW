/*
 * Grammar.cpp
 *
 *  Created on: 10 нояб. 2015 г.
 *      Author: ugin
 */

#include "Grammar.h"

Grammar::Grammar(Node *root) {
	this->root = root;
	rules = getRules( new vector<Rule*>() );
	termDecl = getTermDeclVector( new vector<DeclElem*>() );
	nonTermDecl = getNonTermDeclVector( new vector<DeclElem*>() );
	strToInt = boundNonTerms( new map<string,int>() );
	getFirst();
}

Grammar::~Grammar() {
	unsigned int i;
	for (i=0; i<rules->size(); i++)
		delete rules->at(i);
	for (i=0; i<termDecl->size(); i++)
		delete termDecl->at(i);
	for (i=0; i<nonTermDecl->size(); i++)
		delete nonTermDecl->at(i);
	delete rules;
	delete termDecl;
	delete nonTermDecl;
	delete strToInt;
}

string& Grammar::to_string() {
	unsigned int i;
	unsigned int n;
	strValue = "";
	n = termDecl->size();
	if (n!=0) strValue += "%termDecl ";
	for (i=0; i<n; i++)
		strValue += termDecl->at(i)->to_string() + (i!=n-1?",":".\n");
	n = nonTermDecl->size();
	if (n!=0) strValue += "%nonTermDecl ";
	for (i=0; i<n; i++)
		strValue += nonTermDecl->at(i)->to_string() + (i!=n-1?",":".\n");
	n = rules->size();
	for (i=0; i<n; i++)
		strValue += rules->at(i)->to_string();
	return strValue;
}

vector<DeclElem*>* Grammar::getTermDeclVector(vector<DeclElem*>* tdv) {
	vector<Node*> *nodes = new vector<Node*>();
	root->findNodes(SP_DECL_TERM,nodes);

	if (nodes->size()==0) {
		delete nodes;
		return tdv;
	}

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

vector<DeclElem*>* Grammar::getNonTermDeclVector(vector<DeclElem*>* tdv) {
	vector<Node*> *nodes = new vector<Node*>();
	root->findNodes(SP_DECL_NONTERM,nodes);

	if (nodes->size()==0) {
		delete nodes;
		return tdv;
	}

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

vector<Rule*>* Grammar::getRules(vector<Rule*>* rv) {
	vector<Node*> *nodes = new vector<Node*>();
	root->findNodes(SP_EQUAL,nodes);
	int i;
	for (i=0; i<(int)nodes->size(); i++) {
		Node* rule_NT = (*nodes)[i]->parent;
		RuleFactory factory(rule_NT, this);
		rv->insert(rv->end(), factory.create());
	}
	delete nodes;
	return rv;
}

map<string, int>* Grammar::boundNonTerms(map<string, int>* strToInt) {
	unsigned int i;
	for (i=0; i< rules->size(); i++) {
		string nonTermName(rules->at(i)->lpart->value);
		(*strToInt)[nonTermName] = i;
	}
	return strToInt;
}

void Grammar::getFirst() {
	unsigned int i, n=rules->size();
	bool smthChanged = true;
	cout << "trying to create first sets ..." << "\n";
	while (smthChanged) {
		smthChanged = false;
		for (i=0; i<n; i++) {
			smthChanged = rules->at(i)->getFirst() || smthChanged;
		}
	}
	cout << "first sets have been created" << "\n";
}



DeclElem::DeclElem(YYSTYPE* sym, YYSTYPE* type) {
	this->sym = sym;
	if (type != NULL ) {
		this->attr = string(type->value);
	}
}

DeclElem::~DeclElem() {

}

string& DeclElem::to_string() {
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

RuleFactory::RuleFactory(Node *ruleNode, Grammar *grammar) {
	this->grammar = grammar;
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
	cout << "trying to create factor ..." << "\n";
	Factor * factor = NULL;
	if (parent->children.size()==0)
		factor = new Factor(0, grammar);
	if (parent->children.size()==1) {
		YYSTYPE *ident = parent->children[0]->children[0]->token;
		factor = new Factor(1, ident, grammar);
	}
	if (parent->children.size()==3 && parent->children[0]->token->tag == SP_L_PARENT) {
		MultiAddendum *md = createMultiAddendum(parent->children[1]);
		factor = new Factor(2, md, grammar);
	}
	if (parent->children.size()==3 && parent->children[0]->token->tag == SP_L_SQ_BRACKET) {
		MultiAddendum *md = createMultiAddendum(parent->children[1]);
		factor = new Factor(3, md, grammar);
	}
	if (parent->children.size()==3 && parent->children[0]->token->tag == SP_L_BRACE) {
		MultiAddendum *md = createMultiAddendum(parent->children[1]);
		factor =  new Factor(4, md, grammar);
	}
	cout << "factor created" << "\n";
	return factor;
}


Factor::Factor(int mode, MultiAddendum *m_addendum, Grammar *grammar) {
	this->mode = mode;
	this->m_addendum = m_addendum;
	this->semRule = NULL;
	this->ident = NULL;
	this->grammar = grammar;
}

Factor::Factor(int mode, YYSTYPE *ident, Grammar *grammar) {
	this->mode = mode;
	this->m_addendum = NULL;
	this->semRule = NULL;
	this->ident = ident;
	this->grammar = grammar;
}

Factor::Factor(int mode, Grammar *grammar) {
	this->mode = mode;
	this->m_addendum = NULL;
	this->semRule = NULL;
	this->ident = NULL;
	this->grammar = grammar;
}

Factor::~Factor() {
	if (m_addendum!=NULL)
		delete m_addendum;
}

string& Factor::to_string() {
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

bool Factor::getFirst() {
	cout << "try to create first set for factor ..." << "\n";
	unsigned int n = firstSet.size();
	if (mode==0) {
		firstSet.insert(EMPTY_RULE_TAG);
	}
	if (mode==1) {
		if (ident->tag==SP_NON_TERM) {
			int index = grammar->strToInt->at(string(ident->value));
			Rule *r = grammar->rules->at(index);
			firstSet.insert(r->firstSet.begin(), r->firstSet.end());
		} else if(ident->tag==TERM) {
			firstSet.insert(ident->mark);
		} else throw runtime_error("Generation error! In getFirst() expected TERM");
	}
	if (mode==2) {
		m_addendum->getFirst();
		firstSet.insert(m_addendum->firstSet.begin(), m_addendum->firstSet.end());
	}
	if (mode==3) {
		m_addendum->getFirst();
		firstSet.insert(m_addendum->firstSet.begin(), m_addendum->firstSet.end());
		firstSet.insert(EMPTY_RULE_TAG);
	}
	if (mode==4) {
		m_addendum->getFirst();
		firstSet.insert(m_addendum->firstSet.begin(), m_addendum->firstSet.end());
		firstSet.insert(EMPTY_RULE_TAG);
	}
	cout << "first set for factor has been created" << "\n";
	return n!=firstSet.size();
}

Addendum::Addendum() {

}
Addendum::~Addendum() {
	for (int i=0; i<(int)factors.size(); i++)
		delete factors[i];
}
string& Addendum::to_string() {
	strValue = "";
	for (int i=0; i<(int)factors.size(); i++)
		strValue += " "+factors[i]->to_string();
	return strValue;
}

bool Addendum::getFirst() {
	cout << "try to create first set for addendum..." << "\n";
	unsigned int i, n=factors.size();
	bool smthChanged = false;
	bool continueFlag = true;
	for (i=0; i<n; i++) {
		Factor *f = factors.at(i);
		bool res = f->getFirst();
		if (continueFlag) {
			smthChanged =  res || smthChanged;
			set<int> &s = f->firstSet;
			for (set<int>::iterator it=s.begin(); it!=s.end(); ++it)
				if (*it!=-1)
					firstSet.insert(*it);
		}
		//проверяем наличие пустого правила
		if ( f->firstSet.find(EMPTY_RULE_TAG) == f->firstSet.end() )
			continueFlag = false;
	}
	if (continueFlag) {
		bool res = firstSet.insert(EMPTY_RULE_TAG).second;
		smthChanged = res || smthChanged;
	}
	cout << "first set for addendum has been created" << "\n";
	return smthChanged;
}

MultiAddendum::MultiAddendum() {

}

MultiAddendum::~MultiAddendum( ) {
	for (int i=0; i<(int)addendums.size(); i++)
		delete addendums[i];
}

string& MultiAddendum::to_string() {
	strValue = "";
	for (int i=0; i<(int)addendums.size(); i++)
		strValue += (i==0?"":"|") + addendums[i]->to_string();
	return strValue;
}

bool MultiAddendum::getFirst() {
	cout << "try to create first set for multiAddendum ..." << "\n";
	unsigned int i, n=addendums.size();
	bool smthChanged = false;
	for (i=0; i<n; i++) {
		Addendum *a = addendums.at(i);
		smthChanged = a->getFirst() || smthChanged;
		firstSet.insert(a->firstSet.begin(), a->firstSet.end());
	}
	cout << "first set for multiAddendum has been created" << "\n";
	return smthChanged;
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

string& Rule::to_string() {
	strValue = "";
	strValue += string(Domains[lpart->tag]);
	strValue += " =";
	strValue += rpart->to_string();
	strValue += ".\n";
	return strValue;
}

bool Rule::getFirst() {
	cout << "try to create first set for rule ..." << "\n";
	bool res = rpart->getFirst();
	firstSet.insert(rpart->firstSet.begin(),rpart->firstSet.end());
	cout << "first set for rule has been created" << "\n";
	return res;
}


