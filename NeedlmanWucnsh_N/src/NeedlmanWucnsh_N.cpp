//============================================================================
// Name        : NeedlmanWucnsh_N.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <fstream>

#include "Cube.h"

using namespace std;



int align(string s[], int n, BlossumMatrix* matrix) {
	Cube array(s, n, matrix);
	string *res = array.doAlign();
	for (int i=0; i<n; i++)
		cout << res[i] << "\n";
	delete[] res;
	return array.getMaxScore();
}

string *s;
int n;

void readSeqs(const char *name) {
	/* читается ровно n входных строк
	n - число в первой строке
	каждая i-ая строка является выравниваемой последовательностью */
	int i;
	ifstream stream(name);
	stream >> n;
	s = new string[n];
	for (i=0; i<n; i++) {
		stream >> s[i];
		//cout << s[i] << "\n";
	}
	stream.close();
}

int main() {
	cout << "\n__START__\n";
	readSeqs("Seqs.txt");

	ifstream stream("BlossumMatrix.txt");

	BlossumMatrix *matrix = new BlossumMatrix(stream);

	cout << "score:" << align( s, n, matrix) << "\n";


	stream.close();
	delete matrix;
	delete[] s;

	cout << "\n__END__\n";
	return 0;
}
