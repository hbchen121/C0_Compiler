//
//	Created by hbchen in 2019/04/15
//
#include "myMain.h"
#define isLetter(a) ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || a == '_')

char ch = ' ';
char line[maxLineLength + 1] = {0};
char END_CH = '\0';
int no;
int charptr = 0;
alphabet token;
alphabet keyWords[keyWordsNum];
symbol sy;
symbol keyWordsSymbol[keyWordsNum];


void nextchar(){
	cout << " " << ch;
	ch = line[charptr++];
	if(charptr >= maxLineLength){
		error("Nextchar error!");
	}
}

void insymbol(){
	no = 0;
	while(isspace(ch) || ch == '\n')
		nextchar();
	if(isLetter(ch)){ //identi or keywords
		int k = 0;
		memset(token, 0, sizeof(token)); //clear token
		while(isLetter(ch)){
			token[k++] = ch;
			nextchar();
		}
		//cout << token << endl;
		for(k = 0; k < keyWordsNum; k++){
			//cout << k << endl;
			if(!strcmp(token, keyWords[k])){
				//cout << keyWords[k] << endl;
				sy = keyWordsSymbol[k];
				//cout << keyWordsSymbol[k] << endl;
				//cout << "k:" << k << endl;
				break;
			}
		}
		if(k == keyWordsNum){ //identity
			sy = identi;
			//cout << "\n" << token << endl;	
		}
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
			ch == ')' || ch == '[' || ch == ']' || ch == ',' || ch == ';' ||
			ch == '{' || ch == '}' ){
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
			//cout << ch << ":" << no << endl; 
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
	else if(ch == '='){	// == or =
		nextchar();
		if(ch == '='){
			sy = eql;
			nextchar();
		}
		else{
			sy = becomes;
			no = (int)ch;
		}
	}
	else if(ch == '!'){ // !=
		nextchar();
		if(ch == '='){
			sy = neq;
			nextchar();
		}
		else{
			error("Illege char beside '!'!");
		}
	}
	else if(ch == '>'){	// > or >=
		nextchar();
		if(ch == '='){
			sy = geq;
			nextchar();
		}
		else{
			sy = gtr;
		}

	}
	else if(ch == '<'){
		nextchar();
		if(ch == '='){
			sy = leq;
			nextchar();
		}
		else{
			sy = lss;
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



