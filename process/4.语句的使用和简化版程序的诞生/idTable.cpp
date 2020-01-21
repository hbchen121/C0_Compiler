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
		else if(kd == vars || kd == params){ 
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
			//No function in local
		}
	}
	strcpy(element.name, name);
	element.kd = kd;
	element.typ = typ;
	element.length = length;
	element.level = level;
	idTable.push_back(element);
	if(level == 0){ //global is static
		staticIdTable.push_back(element);
	}
	
};

// return the identity name's idTable index
int lookup(const char name[]){
	subTableElement e = subGrammarTable[subGrammar];
	// current domain search
	for(int i = e.idPtr; i < e.idPtr + e.idNum; i++){
		if(strcmp(name, idTable[i].name) == 0){
			return i;	// find and return index
		}
	}
	
	if(subGrammar != 0){
		// parent domain search(global search)
		e = subGrammarTable[e.parent];
		for(int i = 0; i < e.idNum; i++){
			if(strcmp(name, idTable[i].name) == 0)
				return i;
		}
	}
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
	cout << "\nidTable" << endl;
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
