//
//	Created by hbchen in 2019/04/15
//

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <set>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::ios;
using std::fstream;
using std::map;
using std::vector;
using std::to_string;
using std::stack;
using std::set;
using std::pair;


// global limit
const int maxStrLength = 1000;
const int maxLineLength = 1000;
const int maxDigit = 15;
const int maxInt = 100000000;
const int maxIdentLength = 100;
const int keyWordsNum = 9; 	//Keywords Number

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
	mainsy, // main 21
	ifsy, elsesy, // if else 22 23
	eql, neq, gtr, geq, lss, leq, // == != > >= < <= 24 25 26 27 28 29
	whilesy, // while 30
	returnsy, // return 31
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
//extern fstream inputFile;
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
void printStaticIdTable();
int findCurFunc();
int lookupFunc(const char funcName[]);
int lookupFuncVar(const char funcName[], const char varName[]);
void insertStatic(string func, kind kd, type typ,string var, int length, int level);
int lookupStaticVar(const char* funcName, const char* varName);
int lookupStatic(const char* funcName);



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
void printSy(symbol sy);

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
// simple program
void program();
void mainDef();
void complexState();
void stateList();
void statement();
void ifState();
void whileState();
void judgement(string &infixString);
void assignState();
// funciton
void globalVarState();
void funcWithReturnDef(); 
void funcWithoutReturnDef();
void parameter(int &count);
void paramList(int &count);
void funcWithReturn(string &infixString);
void funcWithoutReturn();
void paramValueList(int funcIndex);
// return 
void returnState();

// Target code generation
void mipsProgram();
void outputMipsCode(string filaName);

//infixNotation Optimization
void printNewInfixTable();
void outputNewInfix();
void optimizeInfixes();
