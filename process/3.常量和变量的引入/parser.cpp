//
//	Created by hbchen in 2019/04/15
//
//	expression 	2019/04/15
//	const state 2019/04/19
#include "myMain.h"
int level = 0; // different domain has different level function is 1 and other is 0

type tempType;

// ＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
void varState(){
	while(sy == intsy || sy == charsy){
		if(skipFlag){
			skipFlag = false;
		}
		varDef();
		if(sy == semicolon){
			insymbol();
		}
		else if(!skipFlag){
			error("semicolon lost in varState!");
		}
	}
}

// ＜变量定义＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']')
//						{,(＜标识符＞|＜标识符＞'['＜无符号整数＞']' )}
void varDef(){
	symbol tempsy = voidsy;
	do{
		if(skipFlag)
			skipFlag = false;
		if(sy == comma){
			insymbol();	// eat last comma in while circulation
		}
		else{	// receive int/char
			tempsy = sy;
			insymbol();			
		}
		string tempId = token;
		if(isDefinable(tempId.c_str())){
			cout << "token:" <<  tempId << endl;
			insymbol();
			if(sy == lbrack){
				// array
				string tempOperand = "";
				string tempVariable = "";
				insymbol();
				integer(tempOperand);
				if(sy == rbrack){
					insertTable(tempId.c_str(), vars, tempsy == intsy ? ints : chars, -1, stoi(tempOperand), level);
					insertInfix("VAR", tempsy == intsy ? "int" : "char", tempOperand, tempId);
					insymbol();
				}else if(!skipFlag){
					error("right brack lose in var define!");
					symbol nexts[] = {comma, semicolon, rbrace};
					skipUntil(nexts);
				}
			}
			else if(sy == comma || sy == semicolon){
				// identi var
				insertTable(tempId.c_str(), vars,  tempsy == intsy ? ints : chars, -1, 0, level);
				insertInfix("VAR", tempsy == intsy ? "int" : "char", "", tempId);
			}
			else{
				cout << "cur_token:" << tempId << endl;
				cout << sy << endl;
				error("Unknow symbol in varDef!");
				symbol nexts[] = {comma, semicolon, rbrace};
				skipUntil(nexts);
			}
		}
		else{
			error("Identity Name Repeat!");
			symbol nexts[] = {comma, semicolon, rbrace};
			skipUntil(nexts);
		}
	}while(sy == comma);
}

// ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
void constState(){
	while(sy == constsy){
		//cout << "constsy" << endl;
		insymbol();
		if(skipFlag){
			skipFlag = false;
		} 
		constDef();
		if(sy == semicolon){
			insymbol();
		}
		else{
			if(!skipFlag){
				error("lose semicolon \';\'!");	
			}
		}
	}
}

// ＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
//                              | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
void constDef(){
	alphabet tempIdenti = "";
	if(sy == intsy){
		do{
			//cout << "int" << endl;
			insymbol();
			if(sy != identi){
				error("lose identi in const define!");
				symbol nexts[] = {semicolon, rbrace, comma};
				skipUntil(nexts);
			}
			else{
				strcpy(tempIdenti, token);
				insymbol();
			}
			if(!skipFlag && sy != becomes){
				error("lose \'=\' in const int define!");
				symbol nexts[] = {semicolon, rbrace, comma};
				skipUntil(nexts);
			}
			else{
				insymbol();
			}
			string tempInt;
			if(!skipFlag){
				integer(tempInt);
			//bulid table for identi
				if(isDefinable(tempIdenti)){
					insertTable(tempIdenti, consts, ints, stoi(tempInt), 0, level);
					insertInfix("CONST", "int", tempInt, tempIdenti);	
				}
				else{
					error("Identity repeat!");
					symbol nexts[] = {semicolon, rbrace, comma};
					skipUntil(nexts);
				}
			} 
			
		}while(sy == comma);
	}
	else if(sy == charsy){
		do{
			//cout << "char" << endl;			
			insymbol();
			if(sy != identi){
				error("lose identi in const define!");
				symbol nexts[] = {semicolon, rbrace, comma};
				skipUntil(nexts);
			}
			else{
				strcpy(tempIdenti, token);
				insymbol();
			}
			if(!skipFlag && sy != becomes){
				error("lose \'=\' in const char define!");
				symbol nexts[] = {semicolon, rbrace, comma};
				skipUntil(nexts);
			}
			else{
				insymbol();	
			}
			if(!skipFlag && sy != charcon){
				error("char Type Error in const char define!");
				symbol nexts[] = {semicolon, rbrace, comma};
				skipUntil(nexts);
			}
			if(!skipFlag){
				string tempChar = to_string(no);
				int tempNo = no;
				insymbol();
				//bulid table for identi
				if(isDefinable(tempIdenti)){
					insertTable(tempIdenti, consts, chars, tempNo, 0, level);
					insertInfix("CONST", "char", tempChar, tempIdenti);	
				}
				else{
					error("Identity repeat!");
					symbol nexts[] = {semicolon, rbrace, comma};
					skipUntil(nexts);
				}
			}
			
		}while(sy == comma);
	}
}

// ＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
type expression(string &infixString){
	string tempOperand = "";
	string tempVariable = "";
	type tempType = voids;
	type firstType = voids;
	symbol tempsy = voidsy;
	
	if(sy == plus || sy == minus){
		tempsy = sy;
		insymbol();
	}
	
	if(!skipFlag){
		firstType = term(infixString);
	}
	
	if(tempsy == minus){
		tempOperand = infixString;
		tempVariable = createTempVar();
		if(firstType == voids){
			error("FirstTypeVoid in expression");
			firstType = ints;
		}
		insertTable(tempVariable.c_str(), vars, firstType, -1, 0, level); //addr = -1 means it doesn't matter addr
		insertInfix("NEG", "", tempOperand, tempVariable);
		infixString = tempVariable;
	}
	
	while(sy == plus || sy == minus){
		tempsy = sy;
		insymbol();
		tempType = term(tempOperand);
		tempVariable = createTempVar();
		if(tempType != firstType){
			warn("DifferentType in expression");
			tempType = firstType; // align with firstType
		}
		//string temp = (tempsy == plus) ? "ADD" : "SUB";
		//cout << temp << endl;
		insertTable(tempVariable.c_str(), vars, tempType, -1, 0, level); //addr = -1 means it doesn't matter addr
		insertInfix(tempsy == plus ? "ADD" : "SUB", infixString, tempOperand, tempVariable);
		infixString = tempVariable;		
	}
	return firstType;
}

// ＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
type term(string &infixString){
	string tempOperand = "";
	string tempVariable = "";
	type firstType = voids;
	type tempType = voids;
	symbol tempsy = voidsy;
	
	firstType = factor(tempOperand);
	infixString = tempOperand;
	
	while(sy == times || sy == idiv){
		tempsy = sy;
		insymbol();
		tempType = factor(tempOperand);
		tempVariable = createTempVar();
		if(tempType != firstType){
			warn("DifferentType in term");
			tempType = firstType;
		}
		insertTable(tempVariable.c_str(), vars, tempType, -1, 0, level);
		insertInfix(tempsy == times ? "MUL" : "DIV", infixString, tempOperand, tempVariable);
		infixString = tempVariable;
	}
	
	return firstType;
}

// ＜因子＞    ::= ＜标识符＞｜＜标识符＞'['＜表达式＞']'｜＜整数＞|＜字符＞
//					｜＜有返回值函数调用语句＞|'('＜表达式＞')'
type factor(string &infixString){
	type tempType = voids;
	if(sy == identi){ 
		// identi, identi[], func having return;
		string tempOperand = "";
		string tempVariable = "";
		symbol tempsy = voidsy;
		char tempCh = ch;
		int tempCharptr = charptr;
		//handle identi
		infixString = token;
		int tempIndex = lookup(token);
		if(tempIndex == -1){
			//error("Identity not define!");
			warn("Identity not define!");
		}
		else{
			tempType = idTable[tempIndex].typ;
		}
		insymbol();
		if(sy == lbrack){ 
			// identi[expression]
			if(tempIndex != -1 && (idTable[tempIndex].kd == funcs || idTable[tempIndex].length == 0)){
				error("IdentityKindNotMatch");
			}		
			insymbol();
			if(!skipFlag){
				tempType = expression(tempOperand);	
				//cout << "----" << endl;
			}
			if(!skipFlag && tempType != ints){
				warn("TypeError in array!");
				tempType = ints; 
			}
			if(!skipFlag){
				if(sy != rbrack){
					error("lose right bracket!");
					symbol nexts[] = {semicolon, rbrack, comma};
					skipUntil(nexts);
				}
				else{
					tempVariable = createTempVar();
					insertTable(tempVariable.c_str(), vars, tempType, -1, 0, level);
					insertInfix("GETARR", infixString, tempOperand, tempVariable);
					infixString = tempVariable;
					insymbol();
				}	
			}
		}
		else if(sy == lparent){
			// identi() --function
//			ch = tempCh;
//			charptr = tempCharptr;
//			sy = tempsy;
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
			tempType = ints; //function default ints temporary
		}
		else{
			// identi
//			int tempValue = 0; 
//			if(tempIndex != -1){
//				tempType = idTable[tempIndex].typ;
//				tempValue = idTable[tempIndex].addr;
//			}
//			if(tempValue != 0){ //test, no meanings
//				infixString += to_string(tempValue);
//			}
			if(idTable[tempIndex].kd == funcs || idTable[tempIndex].length != 0){
				//funcs name or array identity
				error("IdentityKindNotMatch");
			}
		}
	}
	else if(sy == plus || sy == minus || sy == intcon){ // integer
		if(!skipFlag){
			tempType = integer(infixString);	
		}
	}
	else if(sy == charcon){
		tempType = chars;
		infixString = to_string(no);
		insymbol();
	}
	else if(sy == lparent){ // ( expression )
		insymbol();
		if(skipFlag){
			skipFlag = false;	
		}
		tempType = expression(infixString);
		if(sy == rparent){
			insymbol();
		}
		else{
			error("Expression rparent lose!");
			symbol nexts[] = {semicolon, rbrace};
			skipUntil(nexts);
		}
	}
	else{
		error("Illege Factor!");
		symbol nexts[] = {semicolon, rbrace};
		skipUntil(nexts);
	}
	return tempType; //type
}

// ＜整数＞        ::= ［＋｜－］＜无符号整数＞｜０
type integer(string &infixString){
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
		//cout << sy << endl;
		//cout << plus << endl;
		error("LOSE Integer!");
	}
	if(tempsy == minus){
		if(no == 0){
			error("SIGNED ZERO");
		}
		infixString = to_string(-1 * no);
	}
	return ints;
}

