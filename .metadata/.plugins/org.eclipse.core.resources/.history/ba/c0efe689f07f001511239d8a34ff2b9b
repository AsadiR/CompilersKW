/*
 * BlossumMatrix.h
 *
 *  Created on: 29.10.2015
 *      Author: ugin
 */
#ifndef BLOSSUMMATRIX_H_
#define BLOSSUMMATRIX_H_

#include <map>
#include <iostream>

using namespace std;

class BlossumMatrix {
private:
	int n;
	map<char,int> *charToInt;
	int** matrix;
public:
	BlossumMatrix(istream &stream);
	int getValue(char sym1, char sym2);
	virtual ~BlossumMatrix();
	string toString();
};

#endif /* BLOSSUMMATRIX_H_ */
