//
//	Created by hbchen in 2019/04/15
//
#include "myMain.h"

bool skipFlag = false;

void error(string s){
	cout << "\nError:" << s << endl;
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


