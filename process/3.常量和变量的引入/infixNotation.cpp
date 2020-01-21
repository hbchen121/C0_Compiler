//
//	Created by hbchen in 2019/04/17
//
#include "myMain.h"

std::vector<infixNotation> infixTable;
string outputbuf;
int tempVarIndex = 0;

void insertInfix(string _operator, string operand1, string operand2, string operand3){
	infixNotation tempInfix;
	tempInfix._operator = _operator;
	tempInfix.operand1 = operand1;
	tempInfix.operand2 = operand2;
	tempInfix.operand3 = operand3;
	infixTable.push_back(tempInfix);
	
	if(_operator == "ADD"){
		outputbuf += operand3 + " = " + operand1 +  " + " + operand2 + "\n";
	}
	else if(_operator == "SUB"){
		outputbuf += operand3 + " = " + operand1 + " - " + operand2 + "\n";
	}
	else if(_operator == "MUL"){
		outputbuf += operand3 + " = " + operand1 + " * " + operand2 + "\n";
	}
	else if(_operator == "DIV"){
		outputbuf += operand3 + " = " + operand1 + " / " + operand2 + "\n";
	}
	else if(_operator == "CONST"){ //constState 
		outputbuf += operand1 + "   \t" + operand3 + "   " + operand2 + "\n";
		// type + name + value
	}
	else if(_operator == "GETARR"){	//array value
		outputbuf += operand3 + " = " + operand1 + "[" + operand2 + "]\n";
		// name + array + offset
	}
	else if(_operator == "VAR"){
		if(operand2 != ""){ //array define
			outputbuf += operand1+ "   \t" + operand3 + "[" + operand2 + "]\n";
			//type + name + length
		}
		else{	//general var
			outputbuf += operand1+ "   \t" + operand3 + "   " + operand2 + "\n";
			//type + name + ""
		}
	}
	
	
}

void outputInfix(){
	cout << outputbuf << endl;
	return ;
}

void printInfixTable(){
	for(int i = 0; i < (int)infixTable.size(); i++){
		cout << infixTable[i]._operator << "\t" << infixTable[i].operand1 << "\t" << infixTable[i].operand2 << "\t" << infixTable[i].operand3 << endl;
	}
	return ;
}

string createTempVar(){
	string tempVar = "#t" + to_string(tempVarIndex++);
	return tempVar;
}
