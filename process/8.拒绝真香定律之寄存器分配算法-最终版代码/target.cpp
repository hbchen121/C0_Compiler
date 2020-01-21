//
// Created by hbchen in 2019/05/16
//
#include "myMain.h"
#define INF 0x7fffffff
#define isLetter(a) (a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z' || a =='_')

string mipsCode;
string currentFunc;
int infixIndex = 0;	//infixTable index
const bool Global = true;
const bool Local = false;
const int frameOffset = 12;
int globalAddr = 0;

const bool Src = true;
const bool condition = true;
const int regNum = 10; //t0~t9
int time = 0;
int regTime[regNum];
string reg2var[regNum];
map<string, int> var2reg;

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
		int len = 0;
		for(int i = startIndex + 1; i < staticIdTable.size() && staticIdTable[i].kd != funcs; i++){
			if(staticIdTable[i].kd == vars){
				if(staticIdTable[i].length == 0){
					//pushLocalStack();
					len += 1; 
				}
				else{
					//pushLocalStack(staticIdTable[i].length);
					len += staticIdTable[i].length;
				}
			}
		}
		pushLocalStack(len);
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

// get the register isn't uesd recently
int getNearest(){
	int min = INF, minp = 0;
	for(int i = 0; i < regNum; i++){
		if(regTime[i] <= min){
			min = regTime[i];
			minp = i;
		}
	}
	regTime[minp] = time++;
	if(time == INF){
		for(int i = 0; i < regNum; i++){
			regTime[i] = 0;
		}
		time = 0;
	}
	return minp;
}

int getRegister2(string operand, bool isSrc, bool isCondition){
	int reg = -1;
	int i;
	// serach empty reg
	for(i = 0; i < regNum; i++){
		if(reg2var[i] == "")
			break;
	}
	if(i == regNum){
		// all are filled,spill and get reg
		i = getNearest();
	}
	if(isSrc){
		// if operand is source operand, load it
		operandToRegister(operand, "$t" + to_string(i));
	}
			
	// modify relationship between reg and var in var2reg and reg2var	
	map<string, int>::iterator tmpIt2;
	string orginalVar = reg2var[i];
	tmpIt2 = var2reg.find(orginalVar);
	if(tmpIt2 != var2reg.end()){
		tmpIt2->second = -1;
	}
	tmpIt2 = var2reg.find(operand);
	if(tmpIt2 != var2reg.end()){
		tmpIt2->second = i;
	}
	else{
		var2reg.insert(pair<string, int>(operand, i));
	}
	reg2var[i] = operand;			
	reg = i;
	return reg;
}

int getRegister(string operand, bool isSrc){
	int reg = -1;
	/*
	if(!isIdenti(operand)){
		// operand is num
		int i;
		// get reg
		for(i = 0; i < regNum; i++){
			if(reg2var[i] == "")
				break;
		}
		if(i == regNum){
			// all are filled, get spill reg
			i = getNearest();
		}
		// load num
		operandToRegister(operand, "$t" + to_string(i));
		// modify relationship between reg and var in var2reg and reg2var	
		map<string, int>::iterator tmpIt2;
		string orginalVar = reg2var[i];
		tmpIt2 = var2reg.find(orginalVar);
		if(tmpIt2 != var2reg.end()){
			tmpIt2->second = -1;
		}
		reg2var[i] = "";
		reg = i;
	}
	else{
	*/
		//operand is vars
		map<string, int>::iterator tmpIt;
		tmpIt = var2reg.find(operand);
		if(tmpIt == var2reg.end() || tmpIt->second == -1){
			// var isn't used in reg
			reg = -1;
		}
		else{
			reg = tmpIt->second;
		}
		if(reg == -1){
			//not find in reg, distribute register
			int i;
			// serach empty reg
			for(i = 0; i < regNum; i++){
				if(reg2var[i] == "")
					break;
			}
			if(i == regNum){
				// all are filled,spill and get reg
				i = getNearest();
			}
			if(isSrc){
				// if operand is source operand, load it
				operandToRegister(operand, "$t" + to_string(i));
			}
			
			// modify relationship between reg and var in var2reg and reg2var	
			map<string, int>::iterator tmpIt2;
			string orginalVar = reg2var[i];
			tmpIt2 = var2reg.find(orginalVar);
			if(tmpIt2 != var2reg.end()){
				tmpIt2->second = -1;
			}
			tmpIt2 = var2reg.find(operand);
			if(tmpIt2 != var2reg.end()){
				tmpIt2->second = i;
			}
			else{
				var2reg.insert(pair<string, int>(operand, i));
			}
			reg2var[i] = operand;
			
			reg = i;
		}
	//}
	return reg;
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
			int opd1Reg = getRegister(infix.operand1, Src);
			int opd2Reg = getRegister(infix.operand2, Src);
			int opd3Reg = getRegister(infix.operand3, !Src);
			string reg1 = "$t" + to_string(opd1Reg);
			string reg2 = "$t" + to_string(opd2Reg);
			string reg3 = "$t" + to_string(opd3Reg);
			string cod = "add " + reg3 + ", " + reg1 + ", " + reg2;
			insertCode(cod);
			storeThirdOperand(infix.operand3, reg3);
		}
		else if(infix._operator == "SUB"){			
			int opd1Reg = getRegister(infix.operand1, Src);
			int opd2Reg = getRegister(infix.operand2, Src);
			int opd3Reg = getRegister(infix.operand3, !Src);
			string reg1 = "$t" + to_string(opd1Reg);
			string reg2 = "$t" + to_string(opd2Reg);
			string reg3 = "$t" + to_string(opd3Reg);
			string cod = "sub " + reg3 + ", " + reg1 + ", " + reg2;
			insertCode(cod);
			storeThirdOperand(infix.operand3, reg3);	
		}
		else if(infix._operator == "MUL"){
			int opd1Reg = getRegister(infix.operand1, Src);
			int opd2Reg = getRegister(infix.operand2, Src);
			int opd3Reg = getRegister(infix.operand3, !Src);
			string reg1 = "$t" + to_string(opd1Reg);
			string reg2 = "$t" + to_string(opd2Reg);
			string reg3 = "$t" + to_string(opd3Reg);
			string cod1 = "mult " + reg1 + ", " + reg2;
			string cod2 = "mflo " + reg3;
			insertCode(cod1);
			insertCode(cod2);
			storeThirdOperand(infix.operand3, reg3);
		}
		else if(infix._operator == "DIV"){
			int opd1Reg = getRegister(infix.operand1, Src);
			int opd2Reg = getRegister(infix.operand2, Src);
			int opd3Reg = getRegister(infix.operand3, !Src);
			string reg1 = "$t" + to_string(opd1Reg);
			string reg2 = "$t" + to_string(opd2Reg);
			string reg3 = "$t" + to_string(opd3Reg);
			string cod1 = "div " + reg1 + ", " + reg2;
			string cod2 = "mflo " + reg3;
			insertCode(cod1);
			insertCode(cod2);
			storeThirdOperand(infix.operand3, reg3);
		}
		else if(infix._operator == "ASSIGN"){
			string cod = "";
			if(infix.operand1 == "#RET"){
				// func return
				storeThirdOperand(infix.operand3, "$v0");
			}
			else{
				// modify $t0's relation 
				map<string, int>::iterator tmpIt2;
				string orginalVar = reg2var[0];
				tmpIt2 = var2reg.find(orginalVar);
				if(tmpIt2 != var2reg.end()){
					tmpIt2->second = -1;
				}
				regTime[0] = time++;
				reg2var[0] = infix.operand1;
				if(!isIdenti(infix.operand1)){
					// num
					cod = "li $t0, " + infix.operand1;
					// modify $t0's relation
					//reg2var[0] = "";
				}
				else if(staticIdTable[index1].kd == consts){
					//const
					cod = "li $t0, " + to_string(staticIdTable[index1].addr);
					// modify $t0's relation
					//reg2var[0] = "";
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
					// modify $t0's relation
					//reg2var[0] = infix.operand1;
					
				}
				insertCode(cod);
				storeThirdOperand(infix.operand3, "$t0");
				tmpIt2 = var2reg.find(infix.operand3);
				if(tmpIt2 != var2reg.end()){
					tmpIt2->second = 0;
				}
				else{
					var2reg.insert(pair<string, int>(infix.operand3, 0));
				}
				reg2var[0] = infix.operand3; 
			}
		}
		else if(infix._operator == "GETARR"){
			// modify $t1, $t2's relation
			map<string, int>::iterator tmpIt2;
			string orginalVar;
			for(int i = 1; i <= 2; i++){
				orginalVar = reg2var[i];
				tmpIt2 = var2reg.find(orginalVar);
				if(tmpIt2 != var2reg.end()){
					tmpIt2->second = -1;
				}
				reg2var[i] = "";
			}
			
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
			insertCode("add $t1, $t1, $t2");
			int opd3Reg = getRegister(infix.operand3, !Src);
			string reg3 = "$t" + to_string(opd3Reg);
			insertCode("lw " + reg3 + " , ($t1)");
			storeThirdOperand(infix.operand3, reg3);
		}
		else if(infix._operator == "SETARR"){
			// modify $t0, $t1, $t2, $t3's relation
			map<string, int>::iterator tmpIt2;
			string orginalVar;
			for(int i = 0; i <= 3; i++){
				orginalVar = reg2var[i];
				tmpIt2 = var2reg.find(orginalVar);
				if(tmpIt2 != var2reg.end()){
					tmpIt2->second = -1;
				}
				reg2var[i] = "";
			}
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
			int opd1Reg = getRegister2(infix.operand1, Src, condition);
			string reg1 = "$t" + to_string(opd1Reg);
			insertCode("beq " + reg1 + ", $0, " + infix.operand3);
		}
		else if(infix._operator == "EQL"){
			int opd1Reg = getRegister2(infix.operand1, Src, condition);
			int opd2Reg = getRegister2(infix.operand2, Src, condition);
			int opd3Reg = getRegister2(infix.operand3, !Src, condition);
			string reg1 = "$t" + to_string(opd1Reg);
			string reg2 = "$t" + to_string(opd2Reg);
			string reg3 = "$t" + to_string(opd3Reg);
			string cod = "seq " + reg3 + ", " + reg1 + ", " + reg2;
			insertCode(cod);
			storeThirdOperand(infix.operand3, reg3);
		}
		else if(infix._operator == "NEQ"){
			int opd1Reg = getRegister2(infix.operand1, Src, condition);
			int opd2Reg = getRegister2(infix.operand2, Src, condition);
			int opd3Reg = getRegister2(infix.operand3, !Src, condition);
			string reg1 = "$t" + to_string(opd1Reg);
			string reg2 = "$t" + to_string(opd2Reg);
			string reg3 = "$t" + to_string(opd3Reg);
			string cod = "sne " + reg3 + ", " + reg1 + ", " + reg2;
			insertCode(cod);
			storeThirdOperand(infix.operand3, reg3);
		}
		else if(infix._operator == "GTR"){
			int opd1Reg = getRegister2(infix.operand1, Src, condition);
			int opd2Reg = getRegister2(infix.operand2, Src, condition);
			int opd3Reg = getRegister2(infix.operand3, !Src, condition);
			string reg1 = "$t" + to_string(opd1Reg);
			string reg2 = "$t" + to_string(opd2Reg);
			string reg3 = "$t" + to_string(opd3Reg);
			string cod = "sgt " + reg3 + ", " + reg1 + ", " + reg2;
			insertCode(cod);
			storeThirdOperand(infix.operand3, reg3);
		}
		else if(infix._operator == "GEQ"){
			int opd1Reg = getRegister2(infix.operand1, Src, condition);
			int opd2Reg = getRegister2(infix.operand2, Src, condition);
			int opd3Reg = getRegister2(infix.operand3, !Src, condition);
			string reg1 = "$t" + to_string(opd1Reg);
			string reg2 = "$t" + to_string(opd2Reg);
			string reg3 = "$t" + to_string(opd3Reg);
			string cod = "seg " + reg3 + ", " + reg1 + ", " + reg2;
			insertCode(cod);
			storeThirdOperand(infix.operand3, reg3);

		}
		else if(infix._operator == "LSS"){
			int opd1Reg = getRegister2(infix.operand1, Src, condition);
			int opd2Reg = getRegister2(infix.operand2, Src, condition);
			int opd3Reg = getRegister2(infix.operand3, !Src, condition);
			string reg1 = "$t" + to_string(opd1Reg);
			string reg2 = "$t" + to_string(opd2Reg);
			string reg3 = "$t" + to_string(opd3Reg);
			string cod = "slt " + reg3 + ", " + reg1 + ", " + reg2;
			insertCode(cod);
			storeThirdOperand(infix.operand3, reg3);
		}
		else if(infix._operator == "LEQ"){
			int opd1Reg = getRegister2(infix.operand1, Src, condition);
			int opd2Reg = getRegister2(infix.operand2, Src, condition);
			int opd3Reg = getRegister2(infix.operand3, !Src, condition);
			//cout << opd1Reg << opd2Reg << opd3Reg << endl;
			string reg1 = "$t" + to_string(opd1Reg);
			string reg2 = "$t" + to_string(opd2Reg);
			string reg3 = "$t" + to_string(opd3Reg);
			//cout << reg1 << " " << reg2 << " " << reg3 << endl;
			string cod = "sle " + reg3 + ", " + reg1 + ", " + reg2;
			insertCode(cod);
			storeThirdOperand(infix.operand3, reg3);
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
			int opd3Reg = getRegister(infix.operand3, Src);
			string reg3 = "$t" + to_string(opd3Reg);
			insertCode("addi $sp, $sp, -4");
			insertCode("sw " + reg3 + ", ($sp)");
		}
		else if(infix._operator == "CALL"){
			insertCode("jal " + infix.operand3);
		}
		if(infix._operator == "NEG"){
			int opd2Reg = getRegister(infix.operand2, Src);
			int opd3Reg = getRegister(infix.operand3, !Src);
			string reg1 = "$0";
			string reg2 = "$t" + to_string(opd2Reg);
			string reg3 = "$t" + to_string(opd3Reg);
			string cod = "sub " + reg3 + ", " + reg1 + ", " + reg2;
			insertCode(cod);
			storeThirdOperand(infix.operand3, reg3);
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

void initilization(){
	// initi reg2var
	for(int i = 0; i < regNum; i++){
		reg2var[i] = "";
	}
}

void mipsProgram(){
	initilization();
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
