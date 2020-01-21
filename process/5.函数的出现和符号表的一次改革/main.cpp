//
//	Created by hbchen in 2019/04/15
//
#include "myMain.h"

map<char, symbol> specialSymbol;

void setup(){
	//special symbol global variable initalization
	symbol specialsymbol[] = {plus, minus, times, idiv, lparent, rparent, lbrack, rbrack,
							 comma, semicolon, becomes, lbrace, rbrace,};
	char special[] = {'+', '-', '*', '/', '(', ')', '[', ']',
						 ',', ';', '=', '{', '}'};
	for(int i = 0; i < sizeof(special) / sizeof(special[0]); i++){
		specialSymbol[special[i]] = specialsymbol[i];
	}
	//keyWords global variables initalization
	symbol keywordssymbol[keyWordsNum] = {voidsy, constsy, intsy, charsy, mainsy,
										 ifsy, elsesy, whilesy, returnsy};
	alphabet keywords[keyWordsNum] = {"void", "const", "int", "char", "main", 
									"if", "else", "while", "return"};
	for(int i = 0; i < keyWordsNum; i++){
		keyWordsSymbol[i] = keywordssymbol[i]; 
		strcpy(keyWords[i], keywords[i]);	
	}
}

int main(int argc, char** argv) {
	setup();
	string Sexpression = "(\'A\'+1)*(20+b)+fff(c)*a/(cc[20+2]-45)";
	string SconstState = "const int a=11,b=2;const char c=\'C\',d=\'D\';";
	string SvarState = "int aa,bb,cc[100]; char dd,ee;";
	string Sfunc = "int f(char x, int y){x= 'd'; return (x);} void ff(){;} int fff(char ch){return (c);}";
	string Sif = "if(" + Sexpression + "==1+2){while(bb){bb = bb+1;f('a'+c, a+1);}} else aa = 33+33;";
	string Smain = "void main(){" + SconstState + SvarState + Sif + "}";
	string Sprogram = SconstState + SvarState + Sfunc + Smain;
	string s = Sprogram;
	//s = SconstState + Sexpression;
	s.copy(line, s.size(), 0);
	cout << "input size:" << strlen(line) << endl;
	//cin >> line;
	cout << line << endl;

	//cout << sy << endl;
	string str = "";
	
	// local test
	//addSubGrammarTable(level-1);
	
	// program test
	program();
	
	//VarState test
	//varState();
	
	//constState();
	//expression(str);
	//cout << "experssion:" << str << endl;
	outputInfix();
	printInfixTable();
	printIdTable();
	system("pause");	
	return 0;
}
