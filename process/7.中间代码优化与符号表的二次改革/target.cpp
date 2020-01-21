//
// Created by hbchen in 2019/05/16
//
#include "myMain.h"

#define isLetter(a) (a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z' || a =='_')

string mipsCode;
string currentFunc;
int infixIndex = 0;	//infixTable index
const bool Global = true;
const bool Local = false;
const int frameOffset = 12;
int globalAddr = 0;

int mipsLine = 0;

void outputMipsCode(string fileName){
	cout << mipsCode << endl;
	fstream mipsFile;
	mipsFile.open(fileName.c_str(), ios::out);
	mipsFile << mipsCode;
}

void insertCode(string s){
	//mipsCode += to_string(mipsLine++) + "\t" + s + "\n";
	mipsCode += s + "\n";
	return ;
}

void mipsJ(string s){
	insertCode("j " + s);
}

// main func prepare
void mainHead(){
	insertCode("addi $sp, $sp, " + to_string(-1 * frameOffset));
	insertCode("sw $fp, 8($sp)");
	insertCode("sw $ra, 4($sp)");
	insertCode("addi $fp, $sp, 8");
}

void funcHead(){
	// skip all Paramter
	for(; infixIndex < infixTable.size() && infixTable[infixIndex]._operator == "PARAM"; infixIndex++);
	mainHead();
}

void funcTail(){
	insertCode("lw $ra, -4($fp)");
	insertCode("move $sp, $fp");
	insertCode("lw $fp, ($sp)");
	insertCode("addi $sp, $sp, -4");
	insertCode("jr $ra");
	insertCode("");
}

void pushLocalStack(){
	insertCode("addi $sp, $sp, -4");
}

void pushLocalStack(int len){
	insertCode("addi $sp, $sp, " + to_string(len*-4));
}

void pushGlobalStack(){
	globalAddr++;
}

void pushGlobalStack(int len){
	globalAddr += len;
}

void mipsVarDef(bool Global){
	if(Global){
		// global
		for(int i = 0; i < staticIdTable.size() && staticIdTable[i].kd != funcs; i++){
			if(staticIdTable[i].kd == vars){
				if(staticIdTable[i].length == 0){
					pushGlobalStack();
				}
				else{
					pushGlobalStack(staticIdTable[i].length);
				}
			}
		}
	}
	else{
		// push local var
		int startIndex = lookupStatic(currentFunc.c_str());
		for(int i = startIndex + 1; i < staticIdTable.size() && staticIdTable[i].kd != funcs; i++){
			if(staticIdTable[i].kd == vars){
				if(staticIdTable[i].length == 0){
					pushLocalStack();
				}
				else{
					pushLocalStack(staticIdTable[i].length);
				}
			}
		}
	}
}

inline bool isIdenti(string name) {
    if (isLetter(name[0]) || name[0] == '#') {
        return true;
    }
    return false;
}

inline bool isGlobal(string name){
	if(lookupStatic(name.c_str()) != -1){
		return true;
	}
	return false;
}

int getGlobalOffset(string name){
	int offset = staticIdTable[lookupStatic(name.c_str())].addr * 4;
	return offset;
}

int getLocalOffset(string name){
	int index = lookupStaticVar(currentFunc.c_str(), name.c_str());
	int offset = 0;
	if(staticIdTable[index].kd == vars){
		// vars
		offset = frameOffset + staticIdTable[index].addr * 4;
	}
	else{
		// param
		offset = -4 + staticIdTable[index].addr * 4;
	}
	return -1 * offset;
}

void operandToRegister(string operand, string reg){
	int index = lookupStaticVar(currentFunc.c_str(), operand.c_str());
	string cod;
	if(!isIdenti(operand)){
		// num
		cod = "li " + reg + ", " + operand;
	}
	else if(staticIdTable[index].kd == consts){
		// const variable
		cod = "li " + reg + ", " + to_string(staticIdTable[index].addr);
	}
	else{
		int offset;
		if(isGlobal(operand)){
			offset = getGlobalOffset(operand);
			cod = "lw " + reg + ", " + to_string(offset) + "($gp)";
		}
		else{
			offset = getLocalOffset(operand);
			cod = "lw " + reg + ", " + to_string(offset) + "($fp)";
		}
	}
	insertCode(cod);
	return ;
}

void storeThirdOperand(string operand, string reg){
	int offset;
	string cod = "";
	if(isGlobal(operand)){
		offset = getGlobalOffset(operand);
		cod = "sw " + reg + ", " + to_string(offset) + "($gp)";
	}
	else{
		offset = getLocalOffset(operand);
		cod = "sw " + reg + ", " + to_string(offset) + "($fp)";
	}
	insertCode(cod);
	return ;
}

// main/func statement generate
void funcContent(){
	for(; infixIndex < infixTable.size() && infixTable[infixIndex]._operator == "CONST"; infixIndex++);
	// skip const
	for(; infixIndex < infixTable.size() && infixTable[infixIndex]._operator == "VAR"; infixIndex++);
	// skip Var
	mipsVarDef(Local);
	for(; infixIndex < infixTable.size() && infixTable[infixIndex]._operator != "FUNC"; infixIndex++){
		// handle infixNotation untill next func
		infixNotation infix = infixTable[infixIndex];
		int index1 = lookupStaticVar(currentFunc.c_str(), infix.operand1.c_str());
		int index2 = lookupStaticVar(currentFunc.c_str(), infix.operand2.c_str());
		int index3 = lookupStaticVar(currentFunc.c_str(), infix.operand3.c_str());
		if(infix._operator == "ADD"){
			operandToRegister(infix.operand1, "$t1");
			operandToRegister(infix.operand2, "$t2");
			insertCode("add $t0, $t1, $t2");
			storeThirdOperand(infix.operand3, "$t0");
		}
		else if(infix._operator == "SUB"){
			operandToRegister(infix.operand1, "$t1");
			operandToRegister(infix.operand2, "$t2");
			insertCode("sub $t0, $t1, $t2");
			storeThirdOperand(infix.operand3, "$t0");
		}
		else if(infix._operator == "MUL"){
			operandToRegister(infix.operand1, "$t1");
			operandToRegister(infix.operand2, "$t2");
			insertCode("mult $t1, $t2");
			insertCode("mflo $t0");
			storeThirdOperand(infix.operand3, "$t0");
		}
		else if(infix._operator == "DIV"){
			operandToRegister(infix.operand1, "$t1");
			operandToRegister(infix.operand2, "$t2");
			insertCode("div $t1, $t2");
			insertCode("mflo $t0");
			storeThirdOperand(infix.operand3, "$t0");
		}
		else if(infix._operator == "ASSIGN"){
			string cod = "";
			if(infix.operand1 == "#RET"){
				// func return
				storeThirdOperand(infix.operand3, "$v0");
			}
			else{
				if(!isIdenti(infix.operand1)){
					// num
					cod = "li $t0, " + infix.operand1;
				}
				else if(staticIdTable[index1].kd == consts){
					//const
					cod = "li $t0, " + to_string(staticIdTable[index1].addr);
				}
				else{
					// variable or Temp or parameter
					int offset;
					if(isGlobal(infix.operand1)){
						offset = getGlobalOffset(infix.operand1);
						cod = "lw $t0, " + to_string(offset) + "($gp)";
					}
					else{
						offset = getLocalOffset(infix.operand1);
						cod = "lw $t0, " + to_string(offset) + "($fp)";
					}
				}
				insertCode(cod);
				storeThirdOperand(infix.operand3, "$t0");
			}
		}
		else if(infix._operator == "GETARR"){
			operandToRegister(infix.operand2, "$t2"); // get Array Index
			if(isGlobal(infix.operand1)){
				insertCode("li $t1, 4");
				insertCode("mult $t2, $t1");
				insertCode("mflo $t2"); 	//get array offset in $t2
				int offsetBase = getGlobalOffset(infix.operand1);	//get array base
				insertCode("add $t1, $gp, " + to_string(offsetBase));
			}
			else{
				insertCode("li $t1, -4");
				insertCode("mult $t2, $t1");
				insertCode("mflo $t2"); 	//get array offset in $t2
				int offsetBase = getLocalOffset(infix.operand1);	//get array base
				insertCode("add $t1, $fp, " + to_string(offsetBase));
			}
			insertCode("add $t3, $t1, $t2");
			insertCode("lw $t0, ($t3)");
			storeThirdOperand(infix.operand3, "$t0");
		}
		else if(infix._operator == "SETARR"){
			operandToRegister(infix.operand1, "$t0");
			operandToRegister(infix.operand2, "$t2"); // get Array Index
			if(isGlobal(infix.operand3)){
				insertCode("li $t3, 4");
				insertCode("mult $t2, $t3");
				insertCode("mflo $t2"); 	//get array offset in $t2
				int offsetBase = getGlobalOffset(infix.operand3);	//get array base
				insertCode("add $t3, $gp, " + to_string(offsetBase));
			}
			else{
				insertCode("li $t3, -4");
				insertCode("mult $t2, $t3");
				insertCode("mflo $t2"); 	//get array offset in $t2
				int offsetBase = getLocalOffset(infix.operand3);	//get array base
				insertCode("add $t3, $fp, " + to_string(offsetBase));
			}
			insertCode("add $t1, $t2, $t3");
			insertCode("sw $t0, ($t1)");	// sw operand1 to Array
		}
		else if(infix._operator == "JMP"){
			insertCode("j " + infix.operand3);
		}
		else if(infix._operator == "BEQ"){ // infixNotation is BEQ judgeResult, 0, Label
			operandToRegister(infix.operand1, "$t1");
			insertCode("beq $t1, $0, " + infix.operand3);
		}
		else if(infix._operator == "EQL"){
			operandToRegister(infix.operand1, "$t1");
			operandToRegister(infix.operand2, "$t2");
			insertCode("seq $t0, $t1, $t2");
			storeThirdOperand(infix.operand3, "$t0");
		}
		else if(infix._operator == "NEQ"){
			operandToRegister(infix.operand1, "$t1");
			operandToRegister(infix.operand2, "$t2");
			insertCode("sne $t0, $t1, $t2");
			storeThirdOperand(infix.operand3, "$t0");
		}
		else if(infix._operator == "GTR"){
			operandToRegister(infix.operand1, "$t1");
			operandToRegister(infix.operand2, "$t2");
			insertCode("sgt $t0, $t1, $t2");
			storeThirdOperand(infix.operand3, "$t0");
		}
		else if(infix._operator == "GEQ"){
			operandToRegister(infix.operand1, "$t1");
			operandToRegister(infix.operand2, "$t2");
			insertCode("sge $t0, $t1, $t2");
			storeThirdOperand(infix.operand3, "$t0");
		}
		else if(infix._operator == "LSS"){
			operandToRegister(infix.operand1, "$t1");
			operandToRegister(infix.operand2, "$t2");
			insertCode("slt $t0, $t1, $t2");
			storeThirdOperand(infix.operand3, "$t0");
		}
		else if(infix._operator == "LEQ"){
			operandToRegister(infix.operand1, "$t1");
			operandToRegister(infix.operand2, "$t2");
			insertCode("sle $t0, $t1, $t2");
			storeThirdOperand(infix.operand3, "$t0");
		}
		else if(infix._operator == "LABEL"){
			insertCode(infix.operand3 + ":");
		}
		else if(infix._operator == "RET"){
			if(infix.operand3 == " "){
				// void return
			}
			else{
				operandToRegister(infix.operand3, "$v0"); // return value to $v0
			}
			insertCode("j " + currentFunc + "Tail");
		}
		else if(infix._operator == "PUSH"){
			operandToRegister(infix.operand3, "$t0");
			insertCode("addi $sp, $sp, -4");
			insertCode("sw $t0, ($sp)");
		}
		else if(infix._operator == "CALL"){
			insertCode("jal " + infix.operand3);
		}
		if(infix._operator == "NEG"){
			operandToRegister(infix.operand2, "$t2");
			insertCode("sub $t0, $0, $t2");
			storeThirdOperand(infix.operand3, "$t0");
		}
	}

}

void mipsMainDef(){
	currentFunc = "main";
	insertCode(currentFunc + ":");
	infixIndex++;
	mainHead();
	funcContent();
	insertCode(currentFunc + "Tail:");
}

void mipsFuncDef(){
	currentFunc = infixTable[infixIndex].operand3;
	insertCode(currentFunc + ":");
	infixIndex++;
	funcHead();
	funcContent();
	insertCode(currentFunc + "Tail:");
	funcTail();
}

void mipsProgram(){
	insertCode(".data");
	// insert

	insertCode(".text");
	//push global variables in global stack
	for(; infixIndex < infixTable.size() && infixTable[infixIndex]._operator == "CONST"; infixIndex++);
	// const
	for(; infixIndex < infixTable.size() && infixTable[infixIndex]._operator == "VAR"; infixIndex++);
	// vars
	mipsVarDef(Global);

	mipsJ("main");	// jump to main
	insertCode("");	// empty line
	//local function def
	for(; infixIndex < infixTable.size() && infixTable[infixIndex]._operator == "FUNC"
		&& infixTable[infixIndex].operand3 != "main"; infixIndex++){
		mipsFuncDef();
	}
	//main def
	mipsMainDef();
}
