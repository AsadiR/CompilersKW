/*
 * Generator.cpp
 *
 *  Created on: 14 нояб. 2015 г.
 *      Author: ugin
 */

#include "Generator.h"

Generator::Generator(string inputGrammarFileName, string logsFileName) {
    try {
		std::ifstream in(inputGrammarFileName.c_str());
		std::string contents((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
		inputGrammar = (char*)contents.c_str();
		logs = new ofstream(logsFileName.c_str(), std::ofstream::out);
		(*logs) << "Generator constructor has been started.\n";
		(*logs) << "The following grammar was received from " << inputGrammarFileName << ":\n";
		(*logs) << "__________________________________\n";
		(*logs) << string(inputGrammar) << "\n";
		(*logs) << "__________________________________\n";
		lexicalAnalysis();
		(*logs).flush();
		(*logs) << "Parsing has been started.\n";
		Parser p(tokens);
		root = p.parse();
		(*logs) << "Parsing has been finished. Following parsing tree was received:\n";
		(*logs) << root->to_string();
		(*logs).flush();
		grammar = new Grammar(root);
		(*logs) << "Following intermediate representation of input grammar has been received:\n";
		(*logs) << "__________________________________\n";
		(*logs) << grammar->to_string();
		(*logs) << "__________________________________\n";
		(*logs).flush();
	} catch (runtime_error &e) {
		if (logs!=NULL) logs->close();
		throw e;
	}
}

void Generator::lexicalAnalysis() {
	int tag ;
	struct Extra extra;
	init_scanner(inputGrammar, &scanner, &extra);
	(*logs) << "Lexical analysis has been started. Following tokens were received:\n";
	do
	{
		YYSTYPE *value = new YYSTYPE;
		YYLTYPE *coord = new YYLTYPE;
		tag = yylex (value, coord, scanner);

		(*logs) << "		" << tokenToStr(value, coord) << "\n";

		tokens.insert(tokens.end(), value);
		coords.insert(coords.end(), coord);
	} while ( tag != 0);
	(*logs) << "Lexical analysis has been finished.\n";
}

Generator::~Generator() {
	logs->close();
	delete logs;
	delete grammar;
	for (vector<YYSTYPE*>::iterator it = tokens.begin() ; it != tokens.end(); ++it) {
		if ((*it)->attr!=NULL) {
			TYPES *t = (*it)->attr;
			if (t->__string__!=NULL) delete t->__string__;
			delete t;
		}
		delete(*it);
	}
	for (vector<YYLTYPE*>::iterator it = coords.begin() ; it != coords.end(); ++it)
		delete(*it);
	delete root;
	destroy_scanner(scanner);
}

void Generator::generateCppParser(string className, string pathToFile) {
	//генерим хэдер: класс, импорты
	//генерим конструктор и деструктор
	//генерим функцию parse
	//генерим для каждого правила приватную функцию (writeRule)
	//полагаем, что аксиома это нулевое правило
	this->className = className;
	fileH = new ofstream((pathToFile + string(".h")).c_str());
	fileCpp = new ofstream((pathToFile + string(".cpp")).c_str());

	string ucName = className;
	transform(ucName.begin(),
			ucName.end(), ucName.begin(), (int (*)(int))toupper);
	string shift("\t");
	string nShift("\t\t");
	string nnShift("\t\t\t");

	// filling of .h file
	(*fileH) << "#ifndef " << ucName << "_H_\n";
	(*fileH) << "#define " << ucName << "_H_\n";
	(*fileH) << "#include " << "\"lexer/lexer.h\"" << "\n";
	(*fileH) << "#include " << "\"Node.h\"" << "\n";
	(*fileH) << "#include " << "<iostream>" << "\n";
	(*fileH) << "#include " << "<vector>" << "\n";
	(*fileH) << "#include " << "<stdexcept>" << "\n";
	(*fileH) << "using namespace std;" << "\n";

	(*fileH) << "class " << className << " {" << "\n";
	(*fileH) << "private:"  << "\n";
	(*fileH) << shift  << "Node *root;" << "\n";
	(*fileH) << shift  << "unsigned int num;" << "\n";
	(*fileH) << shift  << "vector<YYSTYPE*> tokens;" << "\n";
	(*fileH) << shift  << "YYSTYPE *sym;" << "\n";
	(*fileH) << shift  << "YYSTYPE* next_token();" << "\n";

	vector<Rule*>::iterator it;
	for (it=grammar->rules->begin(); it!=grammar->rules->end(); ++it)
		writeSignature(shift,*it);

	(*fileH) << shift  << "void expected_token(Node *parent, int tag);" << "\n";
	(*fileH) << "public:"  << "\n";
	(*fileH) << shift  << "Node* parse();" << "\n";
	(*fileH) << shift  << className
			<<"(vector<YYSTYPE*> t);" << "\n";
	(*fileH) << shift  << "virtual ~" <<
			className << "();" << "\n";
	(*fileH) << "};" << "\n";
	(*fileH) << "#endif" << "\n";
	(*fileH).flush();

	// filling of cpp file


	//includes
	(*fileCpp) << "#include " << "\"" << className
			<< ".h\"" << "\n";
	//parse()
	(*fileCpp) << "Node* " << className << "::parse() {" << "\n";
	ManagerOfVariables manager;

	if (grammar->initSemRule!=NULL)
			writeSemRule(shift, grammar->initSemRule, manager);

	(*fileCpp) << shift << "root = new Node(NULL, \""
			<<  *(grammar->rules->at(0)->lpart->attr->__string__)
			<< "\");" << "\n";

	(*fileCpp) << shift << "parse_" << *(grammar->rules->at(0)->lpart->attr->__string__)
			<< "(root);" << "\n";

	(*fileCpp) << shift << "if (sym->tag!=0)" << "\n";
	(*fileCpp) << nShift << "throw runtime_error(\"expected EOF!\");" << "\n";

	if (grammar->finalSemRule!=NULL)
				writeSemRule(shift, grammar->finalSemRule, manager);

	(*fileCpp) << shift << "return root;" << "\n";
	(*fileCpp) << "}" << "\n";
	//next_token()
	(*fileCpp) << "YYSTYPE* " << className  << "::next_token() {" << "\n";
	(*fileCpp) << shift << "if (num<tokens.size())" << "\n";
	(*fileCpp) << nShift << "return tokens[++num];" << "\n";
	(*fileCpp) << shift << "else" << "\n";
	(*fileCpp) << nShift << "return tokens[num];" << "\n";
	(*fileCpp) << "}" << "\n";
	//expected_token(Node *parent, int tag)
	(*fileCpp) << "void " << className
			<< "::expected_token(Node *parent, int tag) {" << "\n";
	(*fileCpp) << shift << "if (sym->tag == tag) {" << "\n";
	(*fileCpp) << nShift << "sym = next_token();" << "\n";
	(*fileCpp) << shift << "} else {" << "\n";
	(*fileCpp) << nShift << "throw runtime_error(" << "\n";
	(*fileCpp) << nnShift << "\"Unexpected_token:\"" << "\n";
	(*fileCpp) << nnShift << "+string(Domains[sym->tag])" << "\n";
	(*fileCpp) << nnShift << "+\" instead of: \"" << "\n";
	(*fileCpp) << nnShift << "+string(Domains[tag])" << "\n";
	(*fileCpp) << nnShift << "+\"\\n\");" << "\n";
	(*fileCpp) << shift << "}" << "\n";
	(*fileCpp) << "}" << "\n";
	//конструктор
	(*fileCpp) << className << "::" << className << "(vector<YYSTYPE*> t) {\n";
	(*fileCpp) << shift << "root = NULL;" << "\n";
	(*fileCpp) << shift << "num = 0;" << "\n";
	(*fileCpp) << shift << "this->tokens = t;" << "\n";
	(*fileCpp) << shift << "sym = tokens[0];" << "\n";
	(*fileCpp) << "}" << "\n";
	//диструктор
	(*fileCpp) << className << "::~" << className << "() {}\n";



	for (it=grammar->rules->begin(); it!=grammar->rules->end(); ++it)
		writeRule(*it);


	fileH->close();
	fileCpp->close();
	delete fileH;
	delete fileCpp;
}

string Generator::getRetType(Rule *r) {
	string *lpart = r->lpart->attr->__string__;
	map<string,DeclElem*> *tdm = grammar->nonTermDecl;
	map<string,DeclElem*>::iterator de = tdm->find(*lpart);
	if (de == tdm->end())
		return string("void");
	else
		return *(de->second->attrTypeToken->attr_type);
}

void Generator::writeSignature(string shift, Rule *r) {
	string *lpart = r->lpart->attr->__string__;
	string retType = getRetType(r);
	(*fileH) << shift << retType << " parse_" << *lpart
			<< "(Node *parent);" << "\n";
}

void Generator::writeRule(Rule *r) {
	//генерим номер
	//генерим правило для раскрытия мулти аддендума etc
	ManagerOfVariables manager;
	string *lpart = r->lpart->attr->__string__;
	string shift("\t");
	string retType = getRetType(r);
	(*fileCpp) << retType << " " << className << "::parse_" << *lpart
			<< "(Node *parent) " << "{\n";

	if (retType!="void")
		(*fileCpp) << shift << retType << " " << "result" << ";" << "\n";

	if (r->semRule!=NULL)
		writeSemRule(shift, r->semRule, manager);

	writeMultiAddendum("\t", r->rpart, manager);

	if (retType!="void")
		(*fileCpp) << shift << "return result" << ";" << "\n";

	(*fileCpp) << "}\n";
	(*fileCpp).flush();
}

void Generator::writeMultiAddendum(string shift, MultiAddendum *ma, ManagerOfVariables &manager) {
	string nShift(shift + "\t");
	string nnShift(nShift + "\t");
	string nnnShift(nnShift + "\t");
	if (ma->addendums.size()>1) {
		(*fileCpp) << shift << "switch(sym->tag) {\n";
		vector<Addendum*>::iterator it;
		set<int>::iterator a_it;
		bool emptyRuleExsists = false;
		for (it=ma->addendums.begin(); it!=ma->addendums.end(); ++it) {
			for (a_it=(*it)->firstSet.begin(); a_it!=(*it)->firstSet.end(); ++a_it) {
				if (*a_it!=-1) { //пустое правило
					(*fileCpp) << nShift << "case " << Domains[*a_it] << ":\n";
					(*fileCpp) << nnShift << "{" << "\n";
					writeAddendum(nnnShift,(*it),manager);
					(*fileCpp) << nnnShift << "break;\n";
					(*fileCpp) << nnShift << "}" << "\n";
				} else {
					emptyRuleExsists = true;
				}
			}
		}
		if (!emptyRuleExsists) {
			(*fileCpp) << nShift << "default:\n";
			(*fileCpp) << nnShift << "throw runtime_error(" << "\n";
			(*fileCpp) << nnnShift << "\"Unexpected_tokenInParseIdent:\"" << "\n";
			(*fileCpp) << nnnShift << "+string(Domains[sym->tag])" << "\n";
			(*fileCpp) << nnnShift << "+\"\\n\");" << "\n";
		}
		(*fileCpp) << shift << "}\n";
	} else if(ma->addendums.size()==1)  {
		writeAddendum(shift, ma->addendums[0],manager);
	} else {
		throw runtime_error("Empty addendum list!\n");
	}
}

void Generator::writeAddendum(string shift, Addendum *a, ManagerOfVariables &manager) {
	vector<Factor*>::iterator it;
	manager.saveState();
	for (it=a->factors.begin(); it!=a->factors.end(); ++it) {
		writeFactor(shift, *it, manager);
		//семантическое правило
	}
	manager.restoreState();
}

void Generator::writeFactor(string shift, Factor *f, ManagerOfVariables &manager) {
	string nShift(shift + "\t");
	set<int>::iterator it;
	unsigned int i,n;
	string str;
	switch (f->mode) {
		case MODE_EMPTY:
			//ничего не делаем
			break;
		case MODE_SIMPLE:
			switch (f->ident->tag) {
				case TERM:
					str = *(f->ident->attr->__string__);
					if ( grammar->termDecl->count(str) > 0 ) {
						DeclElem *de = grammar->termDecl->at(str);
						(*fileCpp) << shift
								<< *(de->attrTypeToken->attr_type)
								<< " " << manager.getNameOfNextVar()
								<< " = " << "*(sym->attr->"
								<< *(de->attrTypeToken->attr->__string__)
								<< ");" << "\n";
					}
					(*fileCpp) << shift
							<< "expected_token(parent->addChild(new Node(parent,sym)), "
							<< Domains[f->ident->mark] <<");\n";
					break;
				case SP_NON_TERM:
					str = *(f->ident->attr->__string__);
					if ( grammar->nonTermDecl->count(str) > 0 ) {
						DeclElem *de = grammar->nonTermDecl->at(str);
						(*fileCpp) << shift << *(de->attrTypeToken->attr_type)
								<< " " << manager.getNameOfNextVar() << " = "
								<< "parse_" << *(f->ident->attr->__string__)
								<< "(parent->addChild(new Node(parent,\"" <<
								*(f->ident->attr->__string__) <<"\")));" << "\n";
					} else {
						(*fileCpp) << shift << "parse_" << *(f->ident->attr->__string__)
								<< "(parent->addChild(new Node(parent,\""
								<< str <<"\")));" << "\n";
					}
					break;
			}
			break;
		case MODE_PARENTS:
			manager.beginNewLayer();
			writeMultiAddendum(shift, f->m_addendum, manager);
			manager.endLayer();
			break;
		case MODE_OPTIONAL:
			manager.beginNewLayer();
			(*fileCpp) << shift << "if (";
			n = f->m_addendum->firstSet.size();
			i = 0;
			for (it=f->m_addendum->firstSet.begin(); it!=f->m_addendum->firstSet.end(); ++it) {
				if (*it!=EMPTY_RULE_TAG) { //пустое правило
					(*fileCpp) << "sym->tag==" << Domains[*it] << (i!=n-1?"||":"");
				}
				i++;
			}
			(*fileCpp) << ") {\n";
			writeMultiAddendum(nShift, f->m_addendum, manager);
			(*fileCpp) << shift << "}" <<"\n";
			manager.endLayer();
			break;
		case MODE_ITERATION:
			manager.beginNewLayer();
			(*fileCpp) << shift << "while (";
			n = f->m_addendum->firstSet.size();
			i = 0;
			for (it=f->m_addendum->firstSet.begin(); it!=f->m_addendum->firstSet.end(); ++it) {
				if (*it!=EMPTY_RULE_TAG) { //пустое правило
					(*fileCpp) << " sym->tag==" << Domains[*it] << (i!=n-1?" ||":"");
				}
				i++;
			}
			(*fileCpp) << ") {\n";
			writeMultiAddendum(nShift, f->m_addendum, manager);
			(*fileCpp) << shift << "}" <<"\n";
			manager.endLayer();
			break;
	}
	if (f->semRule != NULL) {
		//вставка семантического правила в код
		writeSemRule(shift, f->semRule, manager);
	}
}

void Generator::writeSemRule(string shift, YYSTYPE *semRule, ManagerOfVariables &manager) {
	string str = *(semRule->attr->__string__);
	str = str.substr(2, str.length()-4);
	//cout << str << endl;
	stringstream ss;

	//заменяем шифры на переменные
	int i = 0, j = 0;
	while (true) {
		j = i;
		i = str.find('$',i);
		if (i == (int)string::npos) break;
		//записываем часть правила в файл
		ss << str.substr(j, i-j);
		if (str[i+1]=='$') {
			ss << "result";
			i+=2;
			continue;
		}

		j = i;
		while ( isdigit(str[j]) || str[j]=='^' || str[j]=='$' ) j++;
		//cout << j <<":"<<i << endl;
		//cout << str.substr(i,j-i) << endl;
		ss << manager.getNameOfVar(str.substr(i,j-i));
		i = j;
	}
	ss << str.substr(j);

	//форматируем семантическое правило
	//просто удаляем все переводы строки и записываем в одну строчку (сильно упрощенный вариант)
	str = ss.str();
	ss.clear();
	ss.str(string());


	unsigned int p;
	ss << shift;
	for (p=0; p<str.length(); p++) {
		if (str[p]=='\n') continue;
		ss << str[p];
	}
	ss << "\n";
	(*fileCpp) << ss.str();
}

ManagerOfVariables::ManagerOfVariables() {
	num = 1;
	mainPartOfVarName = "value";
	layers.insert(layers.end(), 1);
	saved_layers_size = layers.size();
	saved_num = num;
}

ManagerOfVariables::~ManagerOfVariables() {

}

string ManagerOfVariables::getNameOfNextVar() {
	std::stringstream ss;
	ss << mainPartOfVarName << num++;
	return ss.str();
}

string ManagerOfVariables::getNameOfVar(string cipher) {
	//TODO: overflow on layers
	// k - количество ^
	// n - смещение на уровне
	unsigned int i=0,k=0,n=0;
	stringstream ss;
	if (cipher[0] != '$') throw runtime_error("Expected $ in cipher of variable!");
	while (cipher[k+1]=='^') {
	    k++;
	}

	string strN = cipher.substr(k+1);
	n = atoi(strN.c_str());
	if (n==0) throw runtime_error("Bad integer format in cipher!");

	list<int>::reverse_iterator it = layers.rbegin();
	if (k>layers.size()) throw runtime_error("Error, too much ^ symbols!");

	for (i=0; i<k; i++) ++it;
	int cur_layer = (*it);

/*	cout << k << endl;
	cout << cur_layer << endl;
	cout << n << endl;
	cout << strN << endl;
	cout << "num=" << num << endl;*/

	if (cur_layer+n-1 >= num)
		throw runtime_error("Index of semantic attribute bound of range!");

	ss << mainPartOfVarName << (cur_layer+n-1);

	return ss.str();
}

void ManagerOfVariables::beginNewLayer() {
	layers.push_back(num);
}

void ManagerOfVariables::endLayer() {
	layers.pop_back();
}

void ManagerOfVariables::saveState() {
	saved_num = num;
	saved_layers_size = layers.size();
}

void ManagerOfVariables::restoreState() {
	num = saved_num;
	while ( layers.size() > saved_layers_size ) {
		layers.pop_back();
	}
}


