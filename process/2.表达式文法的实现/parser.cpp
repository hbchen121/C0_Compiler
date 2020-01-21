//
//	Created by hbchen in 2019/04/15
//
#include "myMain.h"

// ＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
void expression(string &infixString){
	string tempOperand = "";
	string tempVariable = "";
	symbol tempsy = voidsy;
	
	if(sy == plus || sy == minus){
		tempsy = sy;
		insymbol();
	}
	
	if(tempsy == minus){
		term(tempOperand);
		tempVariable = createTempVar();
		insertInfix("NEG", "", tempOperand, tempVariable);
		infixString = tempVariable;
	}
	else{
		term(infixString);
	}
	
	while(sy == plus || sy == minus){
		tempsy = sy;
		insymbol();
		term(tempOperand);
		tempVariable = createTempVar();
		//string temp = (tempsy == plus) ? "ADD" : "SUB";
		//cout << temp << endl;
		insertInfix(tempsy == plus ? "ADD" : "SUB", infixString, tempOperand, tempVariable);
		infixString = tempVariable;		
	}
	return ;
}

// ＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
void term(string &infixString){
	string tempOperand = "";
	string tempVariable = "";
	symbol tempsy = voidsy;
	
	factor(tempOperand);
	infixString = tempOperand;
	
	while(sy == times || sy == idiv){
		tempsy = sy;
		insymbol();
		factor(tempOperand);
		tempVariable = createTempVar();
		insertInfix(tempsy == times ? "MUL" : "DIV", infixString, tempOperand, tempVariable);
		infixString = tempVariable;
	}
	
	return ;
}

// ＜因子＞    ::= ＜标识符＞｜＜标识符＞'['＜表达式＞']'｜＜整数＞|＜字符＞｜
// 				   ＜有返回值函数调用语句＞|'('＜表达式＞')'
void factor(string &infixString){
	if(sy == identi){ 
		// identi, identi[], func having return;
		string tempOperand = "";
		string tempVariable = "";
		symbol tempsy = voidsy;
		//handle identi
		insymbol();
		if(sy == lbrack){ 
			// identi[expression]
			infixString = token;
			tempVariable = createTempVar();
			insymbol();
			expression(tempVariable);
			//int tempOffset = stoi(tempVariable);
			// lookup token		
			infixString += "[" + tempVariable + "]";
			if(sy != rbrack){
				error("lose right bracket!");
			}
			insymbol();
		}
		else if(sy == lparent){
			// identi() --function
			infixString = token;
			tempVariable = createTempVar();
			insymbol();
			expression(tempVariable);
			//function(tempVariable)			
			infixString += "(" + tempVariable + ")";
			if(sy != rparent){
				error("lose right bracket!");
			}
			insymbol();
		}
		else{
			// identi
			// lookup
			infixString = token; //no look up
		}
	}
	else if(sy == plus || sy == minus || sy == intcon){ // integer
		integer(infixString);
		
	}
	else if(sy == charcon){
		infixString = to_string(no);
		insymbol();
	}
	else if(sy == lparent){ // ( expression )
		insymbol();
		expression(infixString);
		if(sy == rparent){
			insymbol();
		}
		else{
			error("Expression rparent lose!");
		}
	}
	else{
		error("Illege Factor!");
	}
	return ; //type
}

// ＜整数＞        ::= ［＋｜－］＜无符号整数＞｜０
void integer(string &infixString){
	symbol tempsy = voidsy;
	if(sy == plus || sy == minus){
		tempsy = sy;
		//cout << sy << endl;
		insymbol();
		//cout << sy << endl;
	}
	if(sy == intcon){
		infixString = to_string(no);
		insymbol();
	}
	else{
		cout << sy << endl;
		//cout << plus << endl;
		error("LOSE Integer!");
	}
	if(tempsy == minus){
		if(no == 0){
			error("SIGNED ZERO");
		}
		infixString = to_string(-1 * no);
	}
	return ;
}

