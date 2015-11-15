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
	fileH = new ofstream((parserClassName + string(".h")).c_str());
	fileCpp = new ofstream((parserClassName + string(".cpp")).c_str());
	string ucName=parserClassName;
	transform(ucName.begin(),
			ucName.end(), ucName.begin(), (int (*)(int))toupper);
	string shift("\t");

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

	fileH->close();
	fileCpp->close();
	delete fileH;
	delete fileCpp;
}

void Generator::writeSignature(string shift, Rule *r) {
	string *lpart = r->lpart->attr->__string__;
	map<string,DeclElem*> *tdm = grammar->termDecl;
	map<string,DeclElem*>::iterator de = tdm->find(*lpart);
	string retType;
	if (de == tdm->end())
		retType = string("void");
	else
		retType = typeid(de->second->attr).name();
	(*fileH) << shift << retType << " parse_" << *lpart
			<< "(Node *parent);" << "\n";
}

void Generator::writeRule(int shift) {
	//генерим номер
	//генерим правило для раскрытия мулти аддендума etc
}
void Generator::writeMultiAddendum(int shift) {

}
void Generator::writeAddendum(int shift) {

}
void Generator::writeFactor(int shift) {

}

