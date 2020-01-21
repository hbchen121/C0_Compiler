//
// Created by 2019/05/28
//
#include "myMain.h"
#define isLetter(a) (a == '_' || a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z')

struct DAGNode{
	int number;
	string content;
	set<DAGNode*> parents;
	DAGNode* lchild;
	DAGNode* rchild;
};

int lastNum = 0;
vector<DAGNode*> allNodes;
map<string, int> varNodeTable;	// var2int to differentiate each one
map<string, int> varWithInitial;

set<string> crossingVars;

vector<infixNotation> newInfixTable;
string newOutputBuf;

string currentFunc2;

void printNewInfixTable(){
	for(int i = 0; i < (int)newInfixTable.size(); i++){
		cout << i << "\t" << newInfixTable[i]._operator << "\t" << newInfixTable[i].operand1 << "\t" << newInfixTable[i].operand2 << "\t" << newInfixTable[i].operand3 << endl;
	}
	return ;
}

void outputNewInfix(){
	cout << newOutputBuf << endl;
}

inline bool isManageable(string operator_){
	if(operator_ == "ADD" || operator_ == "SUB" || operator_ == "MUL" || 
		operator_ == "DIV" || operator_ == "NEG" || operator_ == "ASSIGN" || 
		operator_ == "GETARR"){
		return true;	
	}
	return false;
}

inline bool isVarName(string operand){
	if(isLetter(operand[0]) || operand[0] == '#'){
		return true;
	}
	return false;
}

// same as function insertInfix in infixNotation.cpp except infixTable and outputBuf
void insertNewInfix(string _operator, string operand1, string operand2, string operand3){	
	//newInfixTable.push_back(infix);
	string outputbuf = "";
	infixNotation tempInfix;
	tempInfix._operator = _operator;
	tempInfix.operand1 = operand1;
	tempInfix.operand2 = operand2;
	tempInfix.operand3 = operand3;
	newInfixTable.push_back(tempInfix);
	
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
	else if(_operator == "FUNC"){	// function define
		outputbuf += operand2 + " " + operand3 + "()\n";
	}
	else if(_operator == "EQL"){
		outputbuf += operand3 + " = " + operand1 +  " == " + operand2 + "\n";
	}
	else if(_operator == "NEQ"){
		outputbuf += operand3 + " = " + operand1 +  " != " + operand2 + "\n";
	}
	else if(_operator == "GTR"){
		outputbuf += operand3 + " = " + operand1 +  " > " + operand2 + "\n";
	}
	else if(_operator == "GEQ"){
		outputbuf += operand3 + " = " + operand1 +  " >= " + operand2 + "\n";
	}
	else if(_operator == "LSS"){
		outputbuf += operand3 + " = " + operand1 +  " < " + operand2 + "\n";
	}
	else if(_operator == "LEQ"){
		outputbuf += operand3 + " = " + operand1 +  " <= " + operand2 + "\n";
	}
	else if(_operator == "JMP"){
		outputbuf += "jmp " + operand3 + "\n";
	}
	else if(_operator == "BEQ"){
		outputbuf += "goto " + operand3 + " if " + operand1 +  " == " + operand2 + "\n";
	}
	else if(_operator == "BNQ"){
		outputbuf += "goto " + operand3 + " if " + operand1 +  " != " + operand2 + "\n";
	}
	else if(_operator == "LABEL"){
		outputbuf += operand3 + "\n";
	}
	else if(_operator == "ASSIGN"){	// vars assign
		outputbuf += operand3 + " = " + operand1 + "\n";
	}
	else if(_operator == "SETARR"){	//array assign
		outputbuf += operand3 + "[" + operand2 + "]" + " = " + operand1 + "\n";
	}
	else if(_operator == "PARAM"){
		outputbuf += "param " + operand1 + " " + operand3 + "\n";
	}
	else if(_operator == "RET"){
		outputbuf += "return " + operand3 + "\n";
	}
	else if(_operator == "CALL"){
		outputbuf += "call " + operand3 + "\n";
	}
	else if(_operator == "PUSH"){	//parameter input
		outputbuf += "push " + operand3 + "\n";
	}
	else if(_operator == "NEG"){
		outputbuf += operand3 + " = "+  "-" + operand2 + "\n";
	}
	newOutputBuf += outputbuf;
}

void insertOperands(set<string>& varInBlock, infixNotation infix){
	// insert var
	if(isVarName(infix.operand1) && varInBlock.count(infix.operand1) == 0){
		varInBlock.insert(infix.operand1);
	}
	if(isVarName(infix.operand2) && varInBlock.count(infix.operand2) == 0){
		varInBlock.insert(infix.operand2);
	}
	if(isVarName(infix.operand3) && varInBlock.count(infix.operand3) == 0){
		varInBlock.insert(infix.operand3);
	}
}

// count vars in the last Block
void checkVarExistenceCount(map<string, int>& varExistenceCount, set<string>& varInBlock){
	set<string>::iterator it;
	for(it = varInBlock.begin(); it != varInBlock.end(); it++){
		map<string, int>::iterator varIt;
		
		//cout << "varInBlock: " << *it << endl;
		
		varIt = varExistenceCount.find(*it);
		if(varIt != varExistenceCount.end()){
			varIt->second ++;
		}
		else{
			varExistenceCount.insert(pair<string, int>(*it, 1));
		}
	}
	//cout << endl;
}

// split all code to different block and get crossing vars
void splitBlocks(){
	map<string, int> varExistenceCount;
	set<string> varInBlock;
	bool inBlock = false;
	for(int i = 0; i < infixTable.size(); i++){
		// count all Variables' Number in diffenrent basic Block
		if(isManageable(infixTable[i]._operator)){
			if(!inBlock){
				// a new basic block
				inBlock = true;
				checkVarExistenceCount(varExistenceCount, varInBlock);
				varInBlock.clear();
			}
		}
		else{
			if(inBlock){
				// a new basic block
				inBlock = false;
				checkVarExistenceCount(varExistenceCount, varInBlock);
				varInBlock.clear();
			}
		}
		// Insert all Variable in its block only once
		if(isManageable(infixTable[i]._operator)
			/*infixTable[i]._operator != "CONST" && infixTable[i]._operator != "VAR" && 
			infixTable[i]._operator != "FUNC" && infixTable[i]._operator != "PARAM" &&
			infixTable[i]._operator != "LABEL" && infixTable[i]._operator != "JMP" */){
			insertOperands(varInBlock, infixTable[i]);	
		}
	}
	
	// Manage the rest infix
	checkVarExistenceCount(varExistenceCount, varInBlock);
	
	// store vars which exist in different block(varExistCount > 1)
	for(map<string, int>::iterator it = varExistenceCount.begin(); 
		it != varExistenceCount.end(); it++){
		if(it->second > 1){
			crossingVars.insert(it->first);
			//cout << "crossingVars:" << it->first << endl;
		}	
	}
}

// Find & set node table, return operand number
int setupOperand(string operand){
	int operandNum;
	map<string, int>::iterator tmpIt;
	tmpIt = varNodeTable.find(operand);
	if(tmpIt != varNodeTable.end()){
		// exist
		operandNum = tmpIt->second;
	}
	else{
		// not exist & setup one
		operandNum = lastNum++;
		if(isdigit(operand[0])){
			// Number	
		}
		else{
			// identifer record first index
			varWithInitial.insert(pair<string, int>(operand, operandNum));
		}
		varNodeTable.insert(pair<string, int>(operand, operandNum));
		DAGNode* node;
		node = new DAGNode;
		node->content = operand;
		node->number = operandNum;
		node->lchild = NULL;
		node->rchild = NULL;
		node->parents.clear();
		// insert
		allNodes.push_back(node);
	}
	return operandNum;
}

// Assign Node is Middle but right child is NULL
inline bool isMidNode(DAGNode *node){
	if(node->lchild != NULL && node->rchild != NULL || node->content == "ASSIGN"){
		return true;
	}
	return false;
}

// setup DAG for each infix
void infixToDAGNode(infixNotation infix){
	int lopNum = 0, ropNum = 0, resOpNum = 0;
	string operator_ = infix._operator;
	string loperand = infix.operand1;
	string roperand = infix.operand2;
	string result = infix.operand3;
	
	if(operator_ != "ASSIGN"){
		// infix isn't assignment
		// setup operand
		lopNum = setupOperand(loperand);
		ropNum = setupOperand(roperand);
		
		// setup result
		int i;
		for(i = 0; i < allNodes.size(); i++){
			if(operator_ == allNodes[i]->content && 
				lopNum == allNodes[i]->lchild->number && 
				ropNum == allNodes[i]->rchild->number){
				// current infix's DAG exist
				resOpNum = allNodes[i]->number;
				break;
			}
		}
		
		// DAG not exist
		if(i == allNodes.size()){
			resOpNum = lastNum++;
			// setup current node 
			DAGNode* node;
			node = new DAGNode;
			node->content = operator_;
			node->number = resOpNum;
			node->lchild = allNodes[lopNum];
			node->rchild = allNodes[ropNum];
			node->parents.clear();
			// insert
			allNodes.push_back(node);
			// set child
			allNodes[lopNum]->parents.insert(node);
			allNodes[ropNum]->parents.insert(node);
		}
	}
	else{
		// infix is a assignment
		// setup operand
		lopNum = setupOperand(loperand); // attention: operand1 is to setup, so it's loperand
		
		// setup result
		if(isMidNode(allNodes[lopNum])){
			// DAG is middle node, assign directly
			resOpNum = lopNum;
		}
		else{
			resOpNum = lastNum++;
			// setup current node 
			DAGNode* node;
			node = new DAGNode;
			node->content = operator_;
			node->number = resOpNum;
			node->lchild = allNodes[lopNum];
			node->rchild = NULL;
			node->parents.clear();
			// insert
			allNodes.push_back(node);
			// set child
			allNodes[lopNum]->parents.insert(node);
		}
	}
	
	// result table setting
	map<string, int>::iterator tmpIt;
	tmpIt = varNodeTable.find(result);
	if(tmpIt != varNodeTable.end()){
		// exist
		tmpIt->second = resOpNum;
	}
	else{
		varNodeTable.insert(pair<string, int>(result, resOpNum));
	}
}

// insert DAGnode into calculation queue & cut off its connection with its children
void setMidNode(DAGNode* node, vector<DAGNode*>& queue, set<int>& inQueue){
	DAGNode* lchild = node->lchild;
	DAGNode* rchild = node->rchild;
	
	set<int>::iterator inIt;
	inIt = inQueue.find(node->number);
	if(inIt == inQueue.end()){
		// not in Queue
		queue.push_back(node);	
		inQueue.insert(node->number);
	}

	//Erase parent in children's parents set
	set<DAGNode*>::iterator tmpIt;
	if(lchild != NULL){
		tmpIt = lchild->parents.find(node);
		if(tmpIt != lchild->parents.end()){
			lchild->parents.erase(tmpIt);
		}
	}
	if(rchild != NULL){
		tmpIt = rchild->parents.find(node);
		if(tmpIt != rchild->parents.end()){
			rchild->parents.erase(tmpIt);
		}
	}
	
	// insert each child to queue recursively if it becomes root node
	if(lchild != NULL){
		if(isMidNode(lchild) && lchild->parents.size() == 0){
			setMidNode(lchild, queue, inQueue);
		}
	}
	if(rchild != NULL){
		if(isMidNode(rchild) && rchild->parents.size() == 0){
			setMidNode(rchild, queue, inQueue);
		}
	}
}

// return Node index in allNodes through its nodeNumber
DAGNode* findNodeWithNumber(int number){
	for(int i = 0; i < allNodes.size(); i++){
		if(allNodes[i]->number == number){
			return allNodes[i];
		}
	}
}

void printCalculationQueue(vector<DAGNode*> queue){
	DAGNode* n;
	cout << "-------------Calculation Queue------------" << endl;
	cout << "num\tcont\tlchild\trchild" << endl;
	for(int i = queue.size() - 1; i >= 0; i--){
		n = queue[i];
		cout << n->number << '\t';
		cout << n->content << '\t';
		if(n->lchild)	cout << n->lchild->number << '\t';
		if(n->rchild)	cout << n->rchild->number << '\t';
		cout << endl;
	}
}

// Export infix from DAG
void exportCodesFromDAG(){
	vector<DAGNode*> rootNodes;
	vector<DAGNode*> calculationQueue;
	set<int> inQueue;
	rootNodes.reserve(10000);
	calculationQueue.reserve(10000);
	
	// Find root node
	//cout << "---------------root---------------" << endl;
	for(int i = 0; i < allNodes.size(); i++){
		if(allNodes[i]->parents.size() == 0){
			rootNodes.push_back(allNodes[i]);
			//test
			//cout << allNodes[i]->number << endl;
		}
	}
	
	// Get calculation queue
	while(!rootNodes.empty()){
		setMidNode(rootNodes[0], calculationQueue, inQueue);
		// delete rootNodes[0]
		rootNodes.erase(rootNodes.begin());
	}
	
	// test - print Calculation Queue
	//printCalculationQueue(calculationQueue);
	
	// Output var with initials, e.g. a0 = a
	map<string, int>::iterator iniIt;
	for(iniIt = varWithInitial.begin(); iniIt != varWithInitial.end(); iniIt++){
		map<string, int>::iterator curIt = varNodeTable.find(iniIt->first);
		if(curIt->second != iniIt->second){
			// variable changed
			string varName = iniIt->first;
			string newVarName = varName + "0";
			
			// Insert a0 to static table
			int orginalIdIndex;
			orginalIdIndex = lookupStaticVar(currentFunc2.c_str(), varName.c_str());
			insertStatic(currentFunc2, vars, staticIdTable[orginalIdIndex].typ, newVarName.c_str(), 0, 1); //local
			
			// output "a0 = a"
			insertNewInfix("ASSIGN", varName, " ", newVarName);
			
			// change "a" node content 
			for(int i = 0; i < allNodes.size(); i++){
				if(allNodes[i]->content == varName){
					allNodes[i]->content = newVarName;
					break;
				}
			}
		}
	}
	//cout << "Output Initial end" << endl;
	// Output calculation queue in reverse
	for(int i = calculationQueue.size() - 1; i >= 0; i--){
		int nodeNumber = calculationQueue[i]->number;
		
		// Collect all other varNode using current Node
		map<string, int>::iterator tmpIt;
		set<string> varNodes;
		for(tmpIt = varNodeTable.begin(); tmpIt != varNodeTable.end(); tmpIt++){
			if(nodeNumber == tmpIt->second){
				varNodes.insert(tmpIt->first);
			}
		}
		
		set<string> vars2Stay;
		set<string> vars2Leave;
		set<string>::iterator tmpIt2;
		DAGNode* node = findNodeWithNumber(nodeNumber);
		DAGNode* lchild = node->lchild;
		DAGNode* rchild = node->rchild;
		
		//get crossing var of varNodes into vars2Stay & others into vars2Leave
		for(tmpIt2 = varNodes.begin(); tmpIt2 != varNodes.end(); tmpIt2++){
			set<string>::iterator crossingVarIt;
			crossingVarIt = crossingVars.find(*tmpIt2);
			if(crossingVarIt != crossingVars.end() || (*tmpIt2)[0] != '#'){
				vars2Stay.insert(*tmpIt2);
			}
			else{
				vars2Leave.insert(*tmpIt2);
			}
		}
		
		if(vars2Stay.size() == 0){
			// No Stay var
			string varName;
			if(vars2Leave.size() != 0){
				// choose fisrt to stay 
				varName = *varNodes.begin();
				vars2Stay.insert(varName);
				vars2Leave.erase(vars2Leave.find(varName));
			}
			else{
				// no var corresponding with node, create one
				varName = createTempVar();
				vars2Stay.insert(varName);
				insertStatic(currentFunc2, vars, ints, varName.c_str(), 0, 1); // ints is ok
			}
			
			// Insert new infix
			string operand1, operand2;
			if(lchild == NULL){
				operand1 = " ";
			}
			else{
				operand1 = lchild->content;
			}
			
			if(rchild == NULL){
				operand2 = " ";
			}
			else{
				operand2 = rchild->content;
			}
			insertNewInfix(node->content, operand1, operand2, varName);
		}
		else{
			// has crossing var
			infixNotation infix;
			string varName = *vars2Stay.begin();
			
			string operand1, operand2;
			if(lchild == NULL){
				operand1 = " ";
			}
			else{
				operand1 = lchild->content;
			}
			
			if(rchild == NULL){
				operand2 = " ";
			}
			else{
				operand2 = rchild->content;
			}
			insertNewInfix(node->content, operand1, operand2, varName);
			
			// insert infix of other vars as assignments
			set<string>::iterator tmpIt;
			tmpIt = ++vars2Stay.begin();
			for(; tmpIt != vars2Stay.end(); tmpIt++){
				insertNewInfix("ASSIGN", varName, " ", *tmpIt);
			}
		}
		
		// set node content
		node->content = *vars2Stay.begin();	
		
		// delete other vars
		set<string>::iterator tmpIt3;
		for(tmpIt3 = vars2Leave.begin(); tmpIt3 != vars2Leave.end(); tmpIt3++){
			string varName = *tmpIt3;
			int staticIndex = lookupStaticVar(currentFunc2.c_str(), varName.c_str());
			if(staticIndex != -1){
				staticIdTable.erase(staticIdTable.begin() + staticIndex);
			}
		}
	}
	//cout << "DAG to infix end" << endl;
}

// recalculate addr
void resettleAddress(){
	int varAddrOff = 0, paraAddrOff = 0;
	for(int i = 0; i < staticIdTable.size(); i++){
		tableElement* e = &staticIdTable[i];
		if(e->level == 0){
			continue;
		}
		if(e->kd == funcs){
			varAddrOff = 0, paraAddrOff = 0;
		}
		else if(e->kd == params){
			e->addr = paraAddrOff--;
		}
		else if(e->kd == vars){
			e->addr = varAddrOff++;
		}
	}
}

void printTestInfo(){
	cout << "------------- VarNodeTable----------------" << endl;
	cout << "Name\tNum" << endl;
	for(map<string, int>::iterator it = varNodeTable.begin(); it != varNodeTable.end(); it++){
		cout << it->first << '\t';
		cout << it->second << endl;
	}
	cout << "------------------DAG-----------------" << endl;
	cout << "num\tcont\tlchild\trchild" << endl;
	for(int i = 0; i < allNodes.size(); i++){
		DAGNode* n = allNodes[i];
		cout << n->number << '\t';
		cout << n->content << '\t';
		if(n->lchild)	cout << n->lchild->number << '\t';
		if(n->rchild)	cout << n->rchild->number << '\t';
		cout << endl;
	}	
}

void optimizeInfixes(){
	// get vars which crossing basic block into <set>crossingVars
	splitBlocks();
	cout << "split end!" << endl;
	
	// Draw DAG & operate optimization
	newInfixTable.reserve(infixTable.size());

	for(int i = 0; i < infixTable.size(); i++){
		if(isManageable(infixTable[i]._operator)){
			// infix is manageable and insert DAGnode
			infixToDAGNode(infixTable[i]);
		}
		else{
			// infix isn't manageable
			
			if(infixTable[i]._operator == "FUNC"){
				currentFunc2 = infixTable[i].operand3;
			}
			if(allNodes.size() != 0){
				//printTestInfo();
				// one block ends, optimize DAG
				exportCodesFromDAG();
				// clear memery and re-optimize
				varWithInitial.clear();
				allNodes.clear();
				varNodeTable.clear();
				lastNum = 0;
			}
			insertNewInfix(infixTable[i]._operator, infixTable[i].operand1,
				infixTable[i].operand2, infixTable[i].operand3);			
		}
	}
	
	// handle the rest
	if(allNodes.size() != 0){
		//printTestInfo();
		// one block ends, optimize DAG
		exportCodesFromDAG();
		// clear memery and re-optimize
		varWithInitial.clear();
		allNodes.clear();
		varNodeTable.clear();
	}
	
	cout << "Optimizate end" << endl;
	
	// recalculate addr
	resettleAddress();
	cout << "reAddr end" << endl;
	
	// store new infix to old infix
	infixTable.clear();
	infixTable.resize(newInfixTable.size());
	for(int i = 0; i < newInfixTable.size(); i++){
		infixTable[i] = newInfixTable[i];
	}
	// output 
	cout << "optDAG end" << endl;
}
