//
//	Created by hbchen in 2019/04/15
//
//	expression 	2019/04/15
//	const state 2019/04/19
#include "myMain.h"
#define isCompare(a) (a == eql || a == neq || a == gtr || a == geq || a == lss || a == leq)

int level = 0; // different domain has different level function is 1 and other is 0
int labelIndex = 0; // different if has different result
type tempType;


// ＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］
//					{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞ 
void program(){
	char tempCh = ch;
	symbol tempSy = sy;
	int tempChptr = charptr;
	nextchar();
	insymbol();
	addSubGrammarTable(level-1);
	if(sy == constsy){
		constState();	
	}
	if(sy == intsy || sy == charsy){
		tempCh = ch;
		tempSy = sy;
		tempChptr = charptr;
		insymbol();
		if(sy == identi){
			insymbol();
		}
		else{
			error("Not var or func in Program!");
			charptr = tempChptr;
			ch = tempCh;
			sy = tempSy;
		}
		if(sy == lparent){
			// func
			charptr = tempChptr;
			ch = tempCh;
			sy = tempSy;
		}
		else if(sy == lbrack || sy == comma || sy == semicolon){
			charptr = tempChptr;
			ch = tempCh;
			sy = tempSy;
			globalVarState();	
		}
		else{
			error("Not var or func in Program!");
			charptr = tempChptr;
			ch = tempCh;
			sy = tempSy;
		}
	}
	
	while(sy == intsy || sy == charsy || sy == voidsy){
		// function define
		if(sy == voidsy){
			tempCh = ch;
			tempSy = sy;
			tempChptr = charptr;
			insymbol();
			if(sy == mainsy){
				// next is main
				charptr = tempChptr;
                ch = tempCh;
                sy = tempSy;
                break;
			}
			else{
				charptr = tempChptr;
                ch = tempCh;
                sy = tempSy;
			}
			//cout << "\nvoids func" << endl;
			funcWithoutReturnDef();
			//cout << "\nvoids func" << endl;
		}
		else{
			//cout << "\nint/char func" << endl;
			funcWithReturnDef();
			//cout << "\nint/char func" << endl;
		}
	}
	//..
	//..
	if(sy == voidsy)
		mainDef();
	else{
		error("illegal symbol in program!");
	}
}

// ＜主函数＞    ::= void main'('')' '{'＜复合语句＞'}'
void mainDef(){
	if(sy == voidsy){
		insymbol();
	}else if(!skipFlag){
		error("void lost Error!");
		symbol nexts[] = {rbrace, lbrace, rparent, lparent};
		skipUntil(nexts);	//skip until ( ) { }
	}
	if(sy == mainsy){
		if(isDefinable(token)){
			insertTable(token, funcs, voids, -1, 0, level);	//addr = -1
		}
		else if(!skipFlag){
			error("main id repeat!");
		}
		insymbol();
	}else if(!skipFlag){
		error("Main id lost Error!");
		symbol nexts[] = {rbrace, lbrace};
		skipUntil(nexts);	//skip until { }
	}
	if(sy == lparent){
		insymbol();
		if(sy == rparent){
			insymbol();
		}
		else{
			error("right parent lost Error!");
			symbol nexts[] = {lbrace, rbrace};
			skipUntil(nexts);
		}
	}else if(!skipFlag){
		error("left parent lost Error in main!");
		symbol nexts[] = {lbrace, rbrace};
		skipUntil(nexts);
	}
	level++;
	subGrammar++;	//subGrammer ++ in new function
	addSubGrammarTable(level-1);
	insertInfix("FUNC", "", "void", "main");
	if(sy == lbrace){
		insymbol();
		if(!skipFlag)
			complexState();
		if(sy == rbrace){
			insymbol();
		}else if(!skipFlag){
			error("right brace lost Error in main!");
		}
	}else if(!skipFlag){
		symbol nexts[] = {rbrace};
		skipUntil(nexts);
	}
	level--;
}

// ＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
void complexState(){
	//cout << "\ncomplex State" << endl;
	constState();
	//cout << "con";
	varState();
	//cout << " var";
	stateList();
	//cout << "list\n";
}

// ＜语句列＞   ::= ｛＜语句＞｝
void stateList(){
	//cout << "\n" << sy << endl;
	while(sy == ifsy || sy == lbrace || sy == semicolon		// if stateList({) empty(;)
		|| sy == whilesy || sy == identi || sy == returnsy	// while assign(identi)
		){
		statement();
	}
}

// ＜语句＞    ::= ＜条件语句＞｜＜循环语句＞| '{'＜语句列＞'}'｜
//				＜有返回值函数调用语句＞; |＜无返回值函数调用语句＞;｜
//				＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜
//				＜空＞; |＜情况语句＞｜＜返回语句＞;
void statement(){
	switch(sy){
		case ifsy:
			ifState();
			break;
		case whilesy:
			whileState();
			break;
		case returnsy:{
			returnState();
			if(sy != semicolon){
				error("semicolon lost in statement!");
			}else{
				insymbol();
			}
			break;
		}
			
		case identi:{	// assign or func
			char tempCh = ch;
			int tempCharptr = charptr;
			symbol tempSy = sy;
			alphabet Name;
			strcpy(Name, token);
			insymbol();
			if(sy == lparent){
				// func
				ch = tempCh;
				charptr = tempCharptr;
				sy = tempSy;
				strcpy(token, Name);
				int tempIndex = lookup(Name);
				if(tempIndex != -1){
					if(idTable[tempIndex].typ == voids){
						// void func
						funcWithoutReturn();
					}
					else{
						// int or char func
						string funcRet = "";	// useless
						funcWithReturn(funcRet);
					}
				}
				else{
					error("func identi lost!");
					symbol nexts[] = {semicolon, rbrace};
					skipUntil(nexts);
				}
			}
			else if(sy == becomes || sy == lbrack){
				ch = tempCh;
				charptr = tempCharptr;
				sy = tempSy;
				strcpy(token, Name);
				assignState();
			}
			else{
				error("Illegal statement in Statement!");
				symbol nexts[] = {semicolon, rbrace};
				skipUntil(nexts);
			}
			//cout << "assign" << endl;
			if(sy != semicolon && !skipFlag){
				error("semicolon lost in statement!");
			}else{
				insymbol();
			}
			//printSy(sy);
			break;
		}			
		case lbrace:{
			//cout << "--{--" << endl;
			insymbol();
			stateList();
			if(sy == rbrace){
				insymbol();
			}else if(!skipFlag){
				error("right brace lost Error!");
				symbol nexts[] = {semicolon, lbrace, rbrace};
				skipUntil(nexts);
			}
			break;
		}
		case semicolon:
			insymbol();	//empty statement
			//cout << "\nEmpty Statement!" << endl;
			break;
		default:
			error("statement Error!");
			break;
	}
}

// ＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞
void assignState(){
	string tempId = token;
	string expressionResult = "";
	int tempIndex = -1;
	type tempType = voids;
	skipFlag = false;
	if(sy == identi){
		tempIndex = lookup(tempId.c_str());
		if(tempIndex == -1){
			error("identity not define in assign!");
		}
		insymbol();
	}
	if(sy == becomes){
		// general variable
		insymbol();
		tempType = expression(expressionResult);
		if(tempType != idTable[tempIndex].typ){
			warn("Type Not Match In assignState!");
			tempType = idTable[tempIndex].typ;
		}
		insertInfix("ASSIGN", expressionResult, "", tempId);
	}
	else if(sy == lbrack){
		//array
		insymbol();
		string arrayIndex = "";
		expression(arrayIndex);
		if(sy == rbrack){
			insymbol();
		}else if(!skipFlag){
			error("array right brack lost!");
			symbol nexts[] = {becomes, semicolon, rbrace, lbrace};
			skipUntil(nexts);
		}
		if(sy == becomes){
			insymbol();
		}
		tempType = expression(expressionResult);
		if(tempType != idTable[tempIndex].typ){
			warn("Type Not Match In assignState!");
			tempType = idTable[tempIndex].typ;
		}
		insertInfix("SETARR", expressionResult, arrayIndex, tempId);
	}
	else{
		error("becomes lost in assign!");
		symbol nexts[] = {semicolon, rbrace};
		skipUntil(nexts);
	}
	//cout << "\n Assignment statement" << endl;
}

// ＜条件语句＞  ::=  if '('＜条件＞')'＜语句＞［else＜语句＞］
void ifState(){
	string judgeResult = "";
	string firstLabel = "";	// first label is if-end label
	string secondLabel = "";// second label is if-else-end label
	skipFlag = false;
	if(sy == ifsy){
		insymbol();
	}
	if(sy == lparent){
		insymbol();
	}else if(!skipFlag){
		error("lparent lost Error!");
		symbol nexts[] = {rparent, semicolon, rbrace};
		skipUntil(nexts);
	}
	if(!skipFlag)
		judgement(judgeResult);
	firstLabel = "LABEL" + to_string(labelIndex++);
	insertInfix("BEQ", judgeResult, "0", firstLabel);	// == 0 jmp to firstLabel(else)
	if(sy == rparent){
		insymbol();
	}else if(!skipFlag){
		error("rparent lost Error!");
		symbol nexts[] = {semicolon, rbrace};
		skipUntil(nexts);
	}
	if(!skipFlag){
		statement();
	}
	secondLabel = "LABEL" + to_string(labelIndex++);
	insertInfix("JMP", " ", " ", secondLabel);	// if finished and jump to secondLabel
	insertInfix("LABEL", " ", " ", firstLabel);	// else-start label
	if(sy == elsesy){
		insymbol();
		statement();
	}
	insertInfix("LABEL", " ", " ", secondLabel); //else(if-else)-end label
}

// ＜while循环语句＞   ::=  while '('＜条件＞')'＜语句＞
void whileState(){
	string judgeResult = "";
	string firstLabel = "";
	string secondLabel = "";
	skipFlag = false;
	if(sy == whilesy){
		insymbol();
	}
	firstLabel = "LABEL" + to_string(labelIndex++);		//while-loop
	insertInfix("LABEL", " ", " ", firstLabel);
	if(sy == lparent){
		insymbol();
		judgement(judgeResult);
		if(sy == rparent){
			insymbol();
		}else if(!skipFlag){
			error("right parent lost");
			symbol nexts[] = {lbrace, rbrace};
			skipUntil(nexts);
		}
	}
	else if(!skipFlag){
		error("left parent lost in while State!");
		symbol nexts[] = {rparent, rbrace, lbrace};
		skipUntil(nexts);
	}
	
	secondLabel = "LABEL" + to_string(labelIndex++);	//while-end
	
	insertInfix("BEQ", judgeResult, "0", secondLabel);
	if(!skipFlag)	statement();
	insertInfix("JMP", " ", " ", firstLabel);
	insertInfix("LABEL", " ", " ", secondLabel);
}

// ＜返回语句＞   ::=  return['('＜表达式＞')']
void returnState(){
	//cout << "\nreturn state!" << endl;
	type funcType = idTable[findCurFunc()].typ;
	type returnType = voids;
	string ret = "";
	if(sy == returnsy){
		insymbol();
		if(sy == lparent){
			insymbol();
			returnType = expression(ret);
			if(sy == rparent){
				if(funcType != returnType){
					warn("Type Not Match in Return!");
				}
				insertInfix("RET", "", "", ret);
				insymbol();
			}else if(!skipFlag){
				error("rparent lost in return!");
				symbol nexts[] = {semicolon, rbrace};
				skipUntil(nexts);
			}
			
		}else{
			insertInfix("RET", "", "", " ");
		}
	}else{
		error("Return lost!");
		symbol nexts[] = {semicolon, rbrace};
		skipUntil(nexts);
	}
} 

// ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数＞')''{'＜复合语句＞'}'
void funcWithoutReturnDef(){
	string funcId = "";
	int parameterCount = 0;
	if(sy == voidsy){
		insymbol();
	}else if(!skipFlag){
		error("void lost Error!");
		symbol nexts[] = {rbrace, lbrace, rparent, lparent};
		skipUntil(nexts);	//skip until ( ) { }
	}
	if(sy == identi){
		if(isDefinable(token)){
			funcId = token;
			insertTable(token, funcs, voids, -1, parameterCount, level);
		}
		else if(!skipFlag){
			error("funcWithoutReturn id repeat!");
		}
		insymbol();
	}else if(!skipFlag){
		error("id lost Error!");
		symbol nexts[] = {rbrace, lbrace};
		skipUntil(nexts);	//skip until { }
	}
	level++;
	subGrammar++;	//subGrammer ++ in new function
	addSubGrammarTable(level-1);
	insertInfix("FUNC", "", "void", funcId);
	if(sy == lparent){
		insymbol();
		parameter(parameterCount);
		int idIndex = lookup(funcId.c_str());
		if(idIndex != -1){
			// update parameterCount
			idTable[idIndex].length = parameterCount;
		}
		if(sy == rparent){
			insymbol();
		}
		else{
			error("right parent lost Error!");
			symbol nexts[] = {lbrace, rbrace};
			skipUntil(nexts);
		}
	}else if(!skipFlag){
		error("left parent lost Error!");
		symbol nexts[] = {lbrace, rbrace};
		skipUntil(nexts);
	}	
	if(sy == lbrace){
		insymbol();
		if(!skipFlag)
			complexState();
		if(sy == rbrace){
			insymbol();
		}else if(!skipFlag){
			error("right brace lost Error!");
		}
	}else if(!skipFlag){
		symbol nexts[] = {rbrace};
		skipUntil(nexts);
	}
	level--;
}

// ＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数＞')' '{'＜复合语句＞'}'
// ＜声明头部＞   ::=  int＜标识符＞ | char＜标识符＞
void funcWithReturnDef(){
	string funcId = "";
	symbol tempType = voidsy;
	int parameterCount = 0;
	if(sy == intsy || sy == charsy){
		tempType = sy;
		insymbol();
	}else if(!skipFlag){
		error("int/char lost Error!");
		symbol nexts[] = {rbrace, lbrace, rparent, lparent};
		skipUntil(nexts);	//skip until ( ) { }
	}
	if(sy == identi){
		if(isDefinable(token)){
			funcId = token;
			insertTable(token, funcs, tempType == intsy ? ints : chars, -1, parameterCount, level);
		}
		else if(!skipFlag){
			error("main id repeat!");
		}
		insymbol();
	}else if(!skipFlag){
		error("id lost Error!");
		symbol nexts[] = {rbrace, lbrace};
		skipUntil(nexts);	//skip until { }
	}
	level++;
	subGrammar++;	//subGrammer ++ in new function
	addSubGrammarTable(level-1);
	insertInfix("FUNC", "", tempType == intsy ? "int" : "chars", funcId);
	if(sy == lparent){
		insymbol();
		parameter(parameterCount);
		int idIndex = lookup(funcId.c_str());
		if(idIndex != -1){
			idTable[idIndex].length = parameterCount;
		}
		if(sy == rparent){
			insymbol();
		}
		else{
			error("right parent lost Error!");
			symbol nexts[] = {lbrace, rbrace};
			skipUntil(nexts);
		}
	}else if(!skipFlag){
		error("left parent lost Error in main!");
		symbol nexts[] = {lbrace, rbrace};
		skipUntil(nexts);
	}
	if(sy == lbrace){
		insymbol();
		if(!skipFlag)
			complexState();
		if(sy == rbrace){
			insymbol();
		}else if(!skipFlag){
			error("right brace lost Error in main!");
		}
	}else if(!skipFlag){
		symbol nexts[] = {rbrace};
		skipUntil(nexts);
	}
	level--;
}

// ＜参数＞    ::= ＜参数表＞
void parameter(int &count){
	//cout << "\nparameter transfer" << endl;
	paramList(count);
}

// ＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
void paramList(int &count){
	type tempType = voids;
	stack<tableElement> paraStk;
	tableElement para;
	while(sy == intsy || sy == charsy){
		tempType = sy == intsy ? ints : chars;
		insymbol();
		if(sy == identi){
			count++;
			strcpy(para.name, token), para.kd = params, para.typ = tempType;
			para.addr = -1, para.length = 0, para.level = level;
			paraStk.push(para); // Transfer parameters from right to left by using Stack 
			insertTable(token, params, tempType, -1, 0, level);
			//cout << "\ntoken:" << token << endl;
			//insertInfix("PARAM", tempType == ints ? "int" : "char", "", token);	
		}
		else{
			error("identi lost Error in paramList!");
		}
		insymbol();
		if(sy == comma){
			insymbol();
		}
		else if(sy == rparent){
			break;
		}
		else{
			error("paramList Error!");
			symbol nexts[] = {comma, rparent};
			skipUntil(nexts);
		}
	}
	// Transfer parameters from right to left
	while(!paraStk.empty()){
		para = paraStk.top();
		//insertTable(para.name, para.kd, para.typ, para.addr, para.length, para.level);
		insertInfix("PARAM", para.typ == ints ? "int" : "char", "", para.name);
		paraStk.pop();
	}
}

// ＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
void judgement(string &infixString){
	string compareLeft = "";
	string compareRight = "";
	symbol compareSy = eql;
	if(!skipFlag){
		expression(compareLeft);
	}
	if(isCompare(sy)){
		compareSy = sy;
		insymbol();
		expression(compareRight);
		infixString = createTempVar();
		insertTable(infixString.c_str(), vars, ints, -1, 0, level);
		switch(compareSy){
			case eql:
				insertInfix("EQL", compareLeft, compareRight, infixString);
				break;
			case neq:
				insertInfix("NEQ", compareLeft, compareRight, infixString);
				break;
			case gtr:
				insertInfix("GTR", compareLeft, compareRight, infixString);
				break;
			case geq:
				insertInfix("GEQ", compareLeft, compareRight, infixString);
				break;
			case lss:
				insertInfix("LSS", compareLeft, compareRight, infixString);
				break;
			case leq:
				insertInfix("LEQ", compareLeft, compareRight, infixString);
				break;
		}
	}else{
		infixString = compareLeft;
	}
}

// ＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
void funcWithReturn(string &infixString){
	//cout << "\nfunc with return" << endl;
	string funName = token;
	int index = lookup(token);
	insymbol();
	if(sy == lparent){
		insymbol();
		paramValueList(index);
		if(sy == rparent){
			skipFlag = false;
			insymbol();
		}else if(!skipFlag){
			error("rparent lost in funcWithReturn call!");
			symbol nexts[] = {semicolon, rbrace};
			skipUntil(nexts);
		}
	}
	else if(!skipFlag){
		error("lparent lost in funcWithReturn call!");
		symbol nexts[] = {rparent, semicolon, rbrace};
		skipUntil(nexts);
	}
	insertInfix("CALL", "", "", funName);
	infixString = createTempVar();
	if(index != -1)
		insertTable(infixString.c_str(), vars, idTable[index].typ, -1, 0, level);
	insertInfix("ASSIGN", "#RET", "", infixString);
}

// ＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
void funcWithoutReturn(){
	string funName = token;
	int index = lookup(token);
	insymbol();
	if(sy == lparent){
		insymbol();
		paramValueList(index);
		if(sy == rparent){
			skipFlag = false;
			insymbol();
		}else if(!skipFlag){
			error("rparent lost in funcWithReturn call!");
			symbol nexts[] = {semicolon, rbrace};
			skipUntil(nexts);
		}
	}
	else if(!skipFlag){
		error("lparent lost in funcWithReturn call!");
		symbol nexts[] = {rparent, semicolon, rbrace};
		skipUntil(nexts);
	}
	insertInfix("CALL", "", "", funName);
}

// ＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
void paramValueList(int funcIndex){
	tableElement func = idTable[funcIndex];
	int paramCount = 0;
	string infixString = "";
	type expType = voids;	
	vector<type> typeList;
	vector<string> paraList;
	
	while(sy != rparent){
		expType = expression(infixString);
		typeList.push_back(expType);
		paraList.push_back(infixString);
		if(sy == comma){
			insymbol();
		}
	}
	paramCount = paraList.size();
	if(paramCount < func.length){
		error("parameter input too little!");
	}
	else if(paramCount > func.length){
		error("parameter input too much!");
	}
	
	for(int i = paramCount - 1; i >= 0; i--){
		tableElement param = idTable[funcIndex + i + 1];
		if(param.typ != typeList[i]){
			string funcs = func.name;
			string s = "Type Match Error for " + to_string(i+1) + "st param of func: " + funcs;
			warn(s);
		}
		insertInfix("PUSH", "", "", paraList[i]);
	}
}

// ＜全局变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
void globalVarState(){
	// 考虑出现有返回值的函数定义
	symbol tempSy = sy;
	char tempCh = ch;
	int tempChptr = charptr; 
	while(sy == intsy || sy == charsy){
		tempSy = sy;
		tempCh = ch;
		tempChptr = charptr;
		if(skipFlag){
			skipFlag = false;
		}
		insymbol();
		if(sy == identi){
			insymbol();
		}
		else{
			error("identi lost in global var state!");
			symbol nexts[] = {semicolon};
			skipUntil(nexts);
		}
		if(sy == lparent){
			// funcWithReturn
			charptr = tempChptr;
			ch = tempCh;
			sy = tempSy;
			return;
		}
		else if(!skipFlag){
			charptr = tempChptr;
			ch = tempCh;
			sy = tempSy;
		}
		if(!skipFlag)
			varDef();
		if(sy == semicolon){
			insymbol();
		}
		else if(!skipFlag){
			error("semicolon lost in globalvarState!");
		}
	}
}

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
			//cout << "token:" <<  tempId << endl;
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
				//cout << "cur_token:" << tempId << endl;
				//cout << sy << endl;
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
			//cout << endl << token << endl;
			error("Identity not define in factor!");
			//warn("Identity not define!");
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
			ch = tempCh;
			charptr = tempCharptr;
			sy = tempsy;
			string funcRet = "";
			
			if(!skipFlag){
				funcWithReturn(funcRet);
				int RetIndex = lookup(funcRet.c_str());
				tempType = idTable[RetIndex].typ;
			}
//			if(sy == semicolon){
//				insymbol();
//			}
//			else if(!skipFlag){
//				error("semicolon lost after func call in factor!");
//				symbol nexts[] = {semicolon, rbrace};
//				skipUntil(nexts);
//			}	
			//insymbol();		
			infixString = funcRet;
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

