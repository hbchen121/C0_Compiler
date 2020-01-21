//
//	Created by hbchen in 2019/04/15
//

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::ios;
using std::fstream;
using std::map;
using std::vector;
using std::to_string;

// global limit
const int maxStrLength = 1000;
const int maxLineLength = 100;
const int maxDigit = 15;
const int maxInt = 100000000;
const int maxIdentLength = 100;


typedef enum{
	identi,
	plus, minus, times, idiv, // + - * /(int)
	intcon, charcon, stringcon,
	voidsy,
	lparent, rparent, lbrack, rbrack, // ( ) [ ]
}symbol;
typedef char alphabet[maxIdentLength + 1];

// lexical Global variables
extern char ch; // current char
extern char line[maxLineLength + 1]; // input buffer
extern char END_CH;
extern map<char, symbol> specialSymbol;
extern int no;
extern alphabet token;
extern symbol sy;

// Lexical function
void insymbol();
void nextchar();

// infix Notation
typedef struct{
	string _operator;
	string operand1;
	string operand2;
	string operand3;
}infixNotation;
extern vector<infixNotation> infixTable;
extern int tempVarIndex; //tempVarIndex
string createTempVar(); // create temporary variable
void insertInfix(string _operator, string operand1, string operand2, string operand3);
void outputInfix();
void printInfixTable();

// Error handling
void error(string s);

//parser production function
void expression(string &infixString);
void term(string &infixString);
void factor(string &infixString);
void integer(string &infixString);
