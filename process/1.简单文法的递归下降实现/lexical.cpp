#include "myMain.h"

char ch = ' ';
char line[INPUT_SIZE] = {0};
char *pc = line;
char END_CH = '\0';

void nextchar(){
	cout << ch;
	ch = *pc++;
}

bool Isdigit(char c){
	if(c >= '0' && c <= '9'){
		return true;
	}
	return false;
}


