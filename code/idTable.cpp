//
// Create by hbchen in 2019/04/19
//
#include "myMain.h"

string typeStr[] = {"voids", "ints", "chars"};	// all type in c0
string kindStr[] = {"consts", "vars", "params", "funcs"};	// all kind in c0
string levelStr[] = {"global", "local"};

const int addrOffsetStart = 0;

vector<tableElement> idTable;
vector<tableElement> staticIdTable;
vector<subTableElement> subGrammarTable;

int addrOffset = addrOffsetStart;
int subGrammar = 0; // when Grammar parser or function parser ++
int paraAddr = addrOffsetStart;

void addSubGrammarTable(int parent){
	subTableElement element;
	element.parent = parent;
	element.idNum = 0;
	element.idPtr = (int)idTable.size();
	subGrammarTable.push_back(element);
}

// insert identity IdTable
void insertTable(const char name[], kind kd, type typ, int addr, int length, int level){
	subGrammarTable[subGrammar].idNum++;
	tableElement element;
	if(level == 0){
		//global defination
		if(kd == consts){
			//const addr  is its value
			element.addr = addr;
		}
		else if(kd == vars){ 
			// variable addr is relative address
			element.addr = addrOffset;
			if(length == 0){
				addrOffset++;
			}
			else{	//array
				addrOffset += length;
			}
		}
		else{
			//function or main
			element.addr = 0;
			addrOffset = addrOffsetStart;
		}
		
	}
	else{
		// local
		if(kd == consts){
			//const addr  is absolute address
			element.addr = addr;
		}
		else if(kd == vars){ 
			// variable addr is relative address
			element.addr = addrOffset;
			if(length == 0){
				addrOffset++;
			}
			else{	//array
				addrOffset += length;
			}
		}
		else if(kd == params){
			element.addr = paraAddr--;
		}
		else{
			//No function in local
		}
	}
	strcpy(element.name, name);
	element.kd = kd;
	element.typ = typ;
	element.length = length;	//func is paraMeterCount
	element.level = level;
	idTable.push_back(element);
	staticIdTable.push_back(element);	
};

// lookup func
int lookupStatic(const char* funcName){
	for(int i = 0; i < staticIdTable.size(); i++){
		if(strcmp(funcName, staticIdTable[i].name) == 0 && staticIdTable[i].level == 0){
			return i;
		}
	}
	return -1;
}

// lookup var
int lookupStaticVar(const char* funcName, const char* varName){
	int funcIndex = lookupStatic(funcName);
	// serach in local
	for(int i = funcIndex + 1; i < staticIdTable.size() && staticIdTable[i].kd != funcs; i++){
		if(strcmp(varName, staticIdTable[i].name) == 0){
			return i;
		}
	}
	// search in global
	for(int i = 0; i < staticIdTable.size() && staticIdTable[i].kd != funcs; i++){
		if(strcmp(varName, staticIdTable[i].name) == 0){
			return i;
		}
	}
	return -1;
}

void insertStatic(string func, kind kd, type typ,string var, int length, int level){
	tableElement element;
	int insertIndex;
	// insert var into tail of block
	insertIndex = lookupStatic(func.c_str());
	if(insertIndex != staticIdTable.size() - 1){
		insertIndex++;
	}
	for(; insertIndex < staticIdTable.size() && staticIdTable[insertIndex].kd != funcs; insertIndex++);
	insertIndex--;
	// address will be modified in func-addressReserve
	element.addr = 0;
	strcpy(element.name, var.c_str());
	element.kd = kd;
	element.typ = typ;
	element.length = length;
	element.level = level;
	staticIdTable.insert(staticIdTable.begin() + insertIndex, element);
}

int findCurFunc(){
	for(int i = idTable.size()-1; i >= 0; i--){
		if(idTable[i].kd == funcs){
//			cout << "\nname\tkind\ttype\taddr\tlength\tlevel" << endl;
//			tableElement e = idTable[i];
//			cout << e.name << '\t';
//			cout << kindStr[e.kd] << '\t';
//			cout << typeStr[e.typ] << '\t';
//			cout << e.addr << '\t';
//			cout << e.length << '\t';
//			cout << levelStr[e.level] << endl;;
			return i;
		}
	}
	error("Func Not Find!");
	exit(0);
	return -1;
}

int lookupFunc(const char funcName[]){
	for(int i = 0; i < (int)idTable.size(); i++){
		if(idTable[i].level == 0 && strcmp(funcName, idTable[i].name) == 0){
			return i;	// find and return index
		}
	}
	return -1;
}

int lookupFuncVar(const char funcName[], const char varName[]){
	int startIndex = lookupFunc(funcName);
	if(startIndex == -1)
		return -1;
	// local var search
	for(int i = startIndex + 1; i < idTable.size() && idTable[i].kd != funcs; i++){
		if(strcmp(varName, idTable[i].name) == 0){
			return i;
		}
	}
	return	lookupFunc(varName); // global var search
}

// return the identity name's idTable index
int lookup(const char name[]){
	//cout << "\n________LOOK UP START_________" << endl;
	subTableElement e = subGrammarTable[subGrammar];
	// current domain search
	for(int i = e.idPtr; i < e.idPtr + e.idNum; i++){
		//cout << endl << idTable[i].name << endl;
		if(strcmp(name, idTable[i].name) == 0){
			return i;	// find and return index
		}
	}
	if(subGrammar != 0){
		// parent domain search(global search)
		//e = subGrammarTable[e.parent];
		for(int i = 0; i < e.idPtr; i++){
			//cout << endl << idTable[i].name << endl;
			if(idTable[i].level == 0 && strcmp(name, idTable[i].name) == 0)
				return i;
		}
	}
	//cout << "\n________LOOK UP END_________" << endl;
	return -1;	//not find and return -1
}

// return wheather the identity name in the idtable
bool isDefinable(const char name[]){
	int t = lookup(name);
	if(t == -1){
		return true; //not find
	}
	else{
		if(idTable[t].level < level){
			return true;	//name has been define but it's in another scope
		}
		else{
			return false;
		}
	}
}

void printIdTable(){
	cout << "\nsubGrammarTable" << endl;
	cout << "parent\tIdNum\tIdPtr" << endl;
	for(int i = 0; i < (int)subGrammarTable.size(); i++){
		subTableElement e = subGrammarTable[i];
		cout << e.parent << '\t';
		cout << e.idNum << '\t';
		cout << e.idPtr << endl;;
	}
	cout << "\n--------------------Id Table-------------------" << endl;
	cout << "name\tkind\ttype\taddr\tlength\tlevel" << endl;
	for(int i = 0; i < (int)idTable.size(); i++){
		tableElement e = idTable[i];
		cout << e.name << '\t';
		cout << kindStr[e.kd] << '\t';
		cout << typeStr[e.typ] << '\t';
		cout << e.addr << '\t';
		cout << e.length << '\t';
		cout << levelStr[e.level] << endl;;
	}
	cout << endl;
}

void printStaticIdTable(){
	cout << "\n----------------Static Id Table-------------------" << endl;
	cout << "name\tkind\ttype\taddr\tlength\tlevel" << endl;
	for(int i = 0; i < (int)staticIdTable.size(); i++){
		tableElement e = staticIdTable[i];
		cout << e.name << '\t';
		cout << kindStr[e.kd] << '\t';
		cout << typeStr[e.typ] << '\t';
		cout << e.addr << '\t';
		cout << e.length << '\t';
		cout << levelStr[e.level] << endl;;
	}
	cout << "-------------------Table end----------------------" << endl;
}
