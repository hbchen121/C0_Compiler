//
//	Created by hbchen in 2019/04/15
//
#include "myMain.h"
#define isLetter(a) ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || a == '_')

char ch = ' ';
char line[maxLineLength + 1] = {0};
symbol sy;
int no;
char END_CH = '\0';
alphabet token;

int charptr = 0;

void nextchar(){
	//cout << ch << endl;
	ch = line[charptr++];
	if(charptr >= maxLineLength){
		error("Nextchar error!");
	}
}

void insymbol(){
	no = 0;
	while(isspace(ch))
		nextchar();
	if(isLetter(ch)){ //identi or keywords
		int k = 0;
		memset(token, 0, sizeof(token));
		while(isLetter(ch)){
			token[k++] = ch;
			nextchar();
		}
		sy = identi;
	}
	else if(isdigit(ch)){ //Integer
		//cout << "Integer" << endl;
		sy = intcon;
		if(ch == '0'){ // 0 or Error
			no = 0;	
			nextchar();
			if(isdigit(ch)){
				error("ZeroDigit!");
				//while(isdigit(ch))
				//	nextchar();
			}
		}
		else{	//Int not 0
			int k = 0;
			no = ch - '0';
			nextchar();
			while(isdigit(ch)){
				no = no * 10 + ch - '0';
				nextchar();
				k++;
			}
			if(k > maxDigit || no > maxInt){
				error("NumRangeExceed!");
				no = 0;
			}
		}
	}
	else if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || 
			ch == ')' || ch == '[' || ch ==']'){
		// specialSymbol
		sy = specialSymbol[ch];
		no = (int)ch;
		nextchar();
	}
	else if(ch == '\''){
		// character
		sy = charcon;
		nextchar();
		if(ch == '\''){
			error("Empty Char");
		}
		else if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || isLetter(ch) || isdigit(ch)){
			no = (int)ch;
			nextchar();
			if(ch != '\''){
				error("Right \' lose!");
				no = 0;
			}
			else
				nextchar();
		}
		else{
			error("Illege char!");
		}

	}
	else if(ch == END_CH){
		cout << "Compiler end!" << endl;
		
	}
	else{
		error("Illege input!");
	}
	//cout << sy << endl;
}



