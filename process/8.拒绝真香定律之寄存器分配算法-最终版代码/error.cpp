//
//	Created by hbchen in 2019/04/15
//
#include "myMain.h"

bool skipFlag = false;

string syString[] = {"identi", "plus", "minus", "times", "idiv", "intcon", "charcon", "stringcon", 
					"voidsy", "lparent", "rparent", "lbrack", "rbrack", "constsy", "intsy", "charsy", 
					"comma", "semicolon", "becomes", "lbrace", "rbrace", "mainsy", "ifsy", "elsesy", 
					"eql", "neq", "gtr", "geq", "lss", "leq", "whilesy", "returnsy"};

void printSy(symbol sy){
	cout << "\nsymbol: " << syString[sy] << endl;
	return ;
}

void error(string s){
	cout << "\ncur_token: " << token << "\ncur_sy: " << sy << " ";
	printSy(sy);
	cout << "\nError:" << s << endl;
//	outputInfix();
//	printInfixTable();
//	printIdTable();
	cout << "--------------------ERROR--------------------" << endl;
	exit(0);
	//skipFlag = true;
	return ;
}

void warn(string s){
	//cout << "\nWarning:" << s << endl;
	return ;
}

bool innexts(symbol nexts[]){
	for(int i = 0; i < sizeof(nexts)/sizeof(nexts[0]); i++){
		if(sy == nexts[i]){
			return true;
		}
	}
	return false;
}

void skipUntil(symbol nexts[]){
	while(!innexts(nexts)){
		insymbol();
	}
	skipFlag = true;
}


