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

void Generator::generateCppParser(string parserClassName) {
	//генерим хэдер: класс, импорты
	//генерим конструктор и деструктор
	//генерим функцию parse
	//генерим для каждого правила приватную функцию (writeRule)
	this->parserClassName = parserClassName;
	fileH = new ofstream((parserClassName + string("_h")).c_str());
	fileCpp = new ofstream((parserClassName + string("_cpp")).c_str());
	string ucName=parserClassName;
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

	(*fileH) << "class " << parserClassName << " {" << "\n";
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
	(*fileH) << shift  << parserClassName
			<<"(vector<YYSTYPE*> t);" << "\n";
	(*fileH) << shift  << "virtual ~" <<
			parserClassName << "();" << "\n";
	(*fileH) << "};" << "\n";
	(*fileH) << "#endif" << "\n";
	(*fileH).flush();

	// filling of cpp file

	//includes
	(*fileCpp) << "#include " << "\"" << parserClassName
			<< ".h\"" << "\n";
	//parse()
	(*fileCpp) << "Node* " << parserClassName << "::parse() {" << "\n";
	(*fileCpp) << shift << "parseGrammar(root)" << "\n";
	(*fileCpp) << shift << "if (sym->tag!=0)" << "\n";
	(*fileCpp) << nShift << "throw runtime_error(\"expected EOF!\");" << "\n";
	(*fileCpp) << shift << "return root;" << "\n";
	(*fileCpp) << "}" << "\n";
	//nextToken()
	(*fileCpp) << "YYSTYPE* " << parserClassName  << "::nextToken() {" << "\n";
	(*fileCpp) << shift << "if (num<tokens.size())" << "\n";
	(*fileCpp) << nShift << "return tokens[++num];" << "\n";
	(*fileCpp) << shift << "else" << "\n";
	(*fileCpp) << nShift << "return tokens[num];" << "\n";
	(*fileCpp) << "}" << "\n";
	//expectedToken(Node *parent, int tag)
	(*fileCpp) << "void " << parserClassName
			<< "::expectedToken(Node *parent, int tag) {" << "\n";
	(*fileCpp) << shift << "if (sym->tag == tag) {" << "\n";
	(*fileCpp) << nShift << "return tokens[++num];" << "\n";
	(*fileCpp) << shift << "} else {" << "\n";
	(*fileCpp) << nShift << "throw runtime_error(" << "\n";
	(*fileCpp) << nnShift << "\"UnexpectedToken:\"" << "\n";
	(*fileCpp) << nnShift << "+string(Domains[sym->tag])" << "\n";
	(*fileCpp) << nnShift << "+\" instead of: \"" << "\n";
	(*fileCpp) << nnShift << "+\"\\n\");" << "\n";
	(*fileCpp) << "}" << "\n";
	//конструктор
	(*fileCpp) << parserClassName << "::" << parserClassName << "() {\n";
	(*fileCpp) << shift << "num = 0;" << "\n";
	(*fileCpp) << shift << "this->tokens = t;" << "\n";
	(*fileCpp) << shift << "sym = tokens[0];" << "\n";
	(*fileCpp) << shift << "root = new Node(NULL);" << "\n";

	(*fileCpp) << "}" << "\n";
	//диструктор
	(*fileCpp) << parserClassName << "::~" << parserClassName << "() {}\n";



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
	string *lpart = r->lpart->attr->__string__;
	string retType = getRetType(r);
	(*fileCpp) << retType << " " << parserClassName << "::parse_" << *lpart
			<< "(Node *parent) " << "{\n";
	writeMultiAddendum("\t", r->rpart);
	(*fileCpp) << "}\n";
	(*fileCpp).flush();
}

void Generator::writeMultiAddendum(string shift, MultiAddendum *ma) {
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
					writeAddendum(nnShift,(*it));
				} else {
					emptyRuleExsists = true;
				}
			}
		}
		if (!emptyRuleExsists) {
			(*fileCpp) << nShift << "default:\n";
			(*fileCpp) << nnShift << "throw runtime_error(" << "\n";
			(*fileCpp) << nnnShift << "\"UnexpectedTokenInParseIdent:\"" << "\n";
			(*fileCpp) << nnnShift << "+string(Domains[sym->tag])" << "\n";
			(*fileCpp) << nnnShift << "+\"\\n\");" << "\n";
		}
		(*fileCpp) << shift << "}\n";
	} else if(ma->addendums.size()==1)  {
		writeAddendum(shift, ma->addendums[0]);
	} else {
		throw runtime_error("Empty addendum list!\n");
	}
}

void Generator::writeAddendum(string shift, Addendum *a) {
	vector<Factor*>::iterator it;
	for (it=a->factors.begin(); it!=a->factors.end(); ++it) {
		writeFactor(shift,*it);
		//семантическое правило
	}
	(*fileCpp) << shift << "break;\n";
}

void Generator::writeFactor(string shift, Factor *f) {
	string nShift(shift + "\t");
	set<int>::iterator it;
	unsigned int i,n;
	switch (f->mode) {
		case 0:
			//ничего не делаем
			break;
		case 1:
			if (f->ident->tag==TERM) {
				(*fileCpp) << shift
						<< "expectedToken(parent->addChild(new Node(parent,sym)), "
						<< Domains[f->ident->mark] <<")\n";
			} else if (f->ident->tag==SP_NON_TERM)  {
				// нужно отдельно рассмотреть случай с невойдовым возвращаемым значением
				(*fileCpp) << shift << "parse_" << *(f->ident->attr->__string__)
						<< "(parent->addChild(new Node(parent)));" << "\n";
			}
			break;
		case 2:
			writeMultiAddendum(shift, f->m_addendum);
			break;
		case 3:
			(*fileCpp) << shift << "if (";
			n = f->m_addendum->firstSet.size();
			i = 0;
			for (it=f->m_addendum->firstSet.begin(); it!=f->m_addendum->firstSet.end(); ++it) {
				if (*it!=-1) { //пустое правило
					(*fileCpp) << Domains[*it] << (i!=n-1?"||":"");
				}
				i++;
			}
			(*fileCpp) << ") {\n";
			writeMultiAddendum(nShift,f->m_addendum);
			(*fileCpp) << shift << "}" <<"\n";
			break;
		case 4:
			(*fileCpp) << shift << "while (";
			n = f->m_addendum->firstSet.size();
			i = 0;
			for (it=f->m_addendum->firstSet.begin(); it!=f->m_addendum->firstSet.end(); ++it) {
				if (*it!=-1) { //пустое правило
					(*fileCpp) << Domains[*it] << (i!=n-1?"||":"");
				}
				i++;
			}
			(*fileCpp) << ") {\n";
			writeMultiAddendum(nShift,f->m_addendum);
			(*fileCpp) << shift << "}" <<"\n";
			break;
	}
}

