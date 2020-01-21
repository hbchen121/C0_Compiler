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
const int maxLineLength = 1000;
const int maxDigit = 15;
const int maxInt = 100000000;
const int maxIdentLength = 100;
const int keyWordsNum = 4; //const state

typedef enum{
	
	identi, // 0
	plus, minus, times, idiv,	// + - * /(int) 1 2 3 4
	intcon, charcon, stringcon, // 5 6 7
	voidsy,	// 8
	lparent, rparent, lbrack, rbrack,	// ( ) [ ] 9 10 11 12
	
	constsy, 	// 13
	intsy, charsy,	// 14 15
	comma, semicolon,	// , ; 16 17
	becomes, // = 18
	
	lbrace, rbrace,	// { } 19 20
}symbol;

typedef char alphabet[maxIdentLength + 1];

// lexical Global variables
extern char ch; // current char
extern int charptr;
extern char line[maxLineLength + 1]; // input buffer
extern char END_CH;
extern map<char, symbol> specialSymbol;
extern int no;
extern alphabet token;
extern alphabet keyWords[keyWordsNum];
extern symbol sy;
extern symbol keyWordsSymbol[keyWordsNum];
// Lexical function
void insymbol();
void nextchar();

// idTable
typedef enum{
	voids, ints, chars,
}type; //all types in C0
typedef enum{
	consts, vars, params, funcs,
}kind; // all kinds in C0
typedef struct{
	alphabet name;	//identi
	kind kd;	//const, variable, parameter, function
	type typ;	//void, int, char
	int addr;
	int length;
	int level; //block level
}tableElement;	//id table element
typedef struct{
	int parent;
	int idNum;
	int idPtr;
}subTableElement;	// subgrammar table
extern vector<tableElement> idTable;
extern vector<tableElement> staticIdTable;
extern vector<subTableElement> subGrammarTable;
extern int subGrammar; // subGarmmar index
extern int addrOffset; // addroffset
void addSubGrammarTable(int parent);
int lookup(const char name[]);
void insertTable(const char name[], kind kd, type tpy,int addr, int length, int level);
bool isDefinable(const char name[]);
void printIdTable();

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
extern bool skipFlag;
void error(string s);
void warn(string s);
void skipUntil(symbol nexts[]);

//parser production function
extern int level;
// expression
type expression(string &infixString);
type term(string &infixString);
type factor(string &infixString);
type integer(string &infixString);
// const state
void constState();
void constDef();
// variable state
void varState();
void varDef();
