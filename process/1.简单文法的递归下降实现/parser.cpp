#include "myMain.h"


void E(string &infixString){
	string tempOperand = "";
	if(ch == '(' || Isdigit(ch)){
		T(tempOperand);
		E_(tempOperand);
		infixString = tempOperand;
	}
	else{
		error("E");
	}
}

void E_(string &infixString){
	string tempVariable = "";
	string tempOperand = "";
	if(ch == '+'){
		nextchar();
		T(tempOperand);
		tempVariable = createTempVar();
		insertInfix("ADD", infixString, tempOperand, tempVariable);
		E_(tempVariable);
		infixString = tempVariable;
	}
	else if(ch == END_CH || ch == ')'){
		return ;
	}
	else{
		error("E_");
	}
};

void T(string &infixString){
	string tempOperand = "";
	if(ch == '(' || Isdigit(ch)){
		F(tempOperand);
		T_(tempOperand);
		infixString = tempOperand;
	}
	else{
		error("T");
	}
};

void T_(string &infixString){
	string tempVariable = "";
	string tempOperand = "";
	if(ch == '*'){
		nextchar();
		F(tempOperand);
		tempVariable = createTempVar();
		insertInfix("MUL", infixString, tempOperand, tempVariable);
		T_(tempVariable);
		infixString = tempVariable;
	}
	else if(ch == '+' || ch == END_CH || ch == ')'){
		return ;
	}
	else{
		error("T_");
	}
};

void F(string &infixString){
	string tempVariable = "";
	string tempOperand = "";
	if(ch == '('){
		nextchar();
		E(infixString);
		if(ch != ')'){
			error("F");
		}
		nextchar();
	}
	else if(Isdigit(ch)){
		//tempVariable = createTempVar();
		//tempOperand = "0";
		string temp(1, ch); // char to string; also can using temp += ch;
		//insertInfix("ADD", temp, tempOperand, tempVariable);
		nextchar();
		infixString = temp; //tempVariable
	}
	else{
		error("F");
	}
};
