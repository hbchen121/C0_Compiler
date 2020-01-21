#include <iostream>
#include <vector>
#include <string>
#define INPUT_SIZE 100
using namespace std;

// input && lexical Global variables
extern char ch; // current char
extern char line[INPUT_SIZE]; // input buffer
extern char* pc;
extern char END_CH;

// Lexical
void nextchar();
bool Isdigit(char c);

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
void E(string &infixString);
void E_(string &infixString);
void T(string &infixString);
void T_(string &infixString);
void F(string &infixString);
