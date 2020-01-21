//
//	Created by hbchen in 2019/04/15
//
#include "myMain.h"

map<char, symbol> specialSymbol;

void setup(){
	symbol specialsymbol[] = {plus, minus, times, idiv, lparent, rparent, lbrack, rbrack};
	char special[] = {'+', '-', '*', '/', '(', ')', '[', ']'};
	for(int i = 0; i < sizeof(special) / sizeof(special[0]); i++){
		specialSymbol[special[i]] = specialsymbol[i];
	}
}

int main(int argc, char** argv) {
	setup();
	string s = "(\'A\'+1)*(20+b[0])+f(c)*a/(d-44)";
	s.copy(line, s.size(), 0);
	//cin >> line;
	cout << line << endl;
	nextchar();
	insymbol();
	string str = "";
	expression(str);
	cout << "experssion:" << str << endl;
	outputInfix();
	printInfixTable();
	system("pause");	
	return 0;
}
