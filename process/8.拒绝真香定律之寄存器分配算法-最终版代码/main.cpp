//
//	Created by hbchen in 2019/04/15
//
#include "myMain.h"

map<char, symbol> specialSymbol;

void setup(){
	//special symbol global variable initalization
	symbol specialsymbol[] = {plus, minus, times, idiv, lparent, rparent, lbrack, rbrack,
							 comma, semicolon, becomes, lbrace, rbrace,};
	char special[] = {'+', '-', '*', '/', '(', ')', '[', ']',
						 ',', ';', '=', '{', '}'};
	for(int i = 0; i < sizeof(special) / sizeof(special[0]); i++){
		specialSymbol[special[i]] = specialsymbol[i];
	}
	//keyWords global variables initalization
	symbol keywordssymbol[keyWordsNum] = {voidsy, constsy, intsy, charsy, mainsy,
										 ifsy, elsesy, whilesy, returnsy};
	alphabet keywords[keyWordsNum] = {"void", "const", "int", "char", "main", 
									"if", "else", "while", "return"};
	for(int i = 0; i < keyWordsNum; i++){
		keyWordsSymbol[i] = keywordssymbol[i]; 
		strcpy(keyWords[i], keywords[i]);	
	}
}

int main(int argc, char** argv) {
	setup();
	/* 
	// all grammar test
	string Sexpression = "(\'A\'+1)*(20+b)+fff(c)*a/(cc[20+2]-45)";
	string SconstState = "const int a=11,b=2;const char c=\'C\',d=\'D\';";
	string SvarState = "int aa,bb,cc[100]; char dd,ee;";
	string Sfunc = "int f(char x, int y){int z; x= 'd'; return (x);} void ff(){;} int fff(char ch){return (c);}";
	string Sif = "if(" + Sexpression + "==1+2){while(bb){bb = bb+1;f('a'+c, a+1);}} else aa = 33+33;";
	string Smain = "void main(){" + SconstState + SvarState + Sif + "}";
	string Sprogram = SconstState + SvarState + Sfunc + Smain;
	string s = Sprogram;
	*/
	
	//cout << s << endl;
	//return 0;
	
	/*
	// main, var and expression test
	//string SconstState = "const int a=11,b=2;const char c=\'C\',d=\'D\';";
	string SvarState = "int aa;int a, b, c;"; //,bb,cc; char dd,ee;"; //"int aa,bb,cc[100]; char dd,ee;";
	string Sexpression = "aa = 1+2*1/2; aaa[1] = aa + 1;a = 1*1+1*1; c = a+1; c = c+1; b = a+1+1;"; //"aa = (\'A\'+1)*(20+bb)+2*a/(cc[20+2]-45)";
	string Sexpre = "int a, b, c;a = 1*1+1*1; b = a+1; c = a + 1 + 1;if(1==1){a = a + 1;}}";
	string test = "int a, b, c;";
	string test1 = "a = 1*1+1*1; c = a+1; c = c+1; b = a+1+1;";
	string Sif = "aa = 0; if(aa + 1 > 0){aaa[0] = aa + 1;}else{aaa[0] = aa + 2;}";
	string SglobalVarState = "int aaa[2], bbb;";
	string SfuncDef = "int f(int x, int y){y = x + 1; return (y);}";
	string Sfunc = "bbb = f(aa, aaa[1]);";
	string Swhile = "aa = 0 ;while(aa < 5){aa = aa + 1;} bbb = aa;";
	//string Smain = "void main(){"  + test + test1 + "}";
	string Smain = "void main(){"  + SvarState + Sexpression + Sfunc + Sif + Swhile + "}";
	string Sprogram = SglobalVarState + SfuncDef + Smain;
	//string Sprogram = Smain;
	string s = Sprogram;
	*/
	char fileName[] = "..\\GCC_MIPS_TEST\\C_test.c";
	fstream inputFile;
	inputFile.open(fileName, ios::in);
	int i = 0;
	char tmpCh;
	while(inputFile.peek() != EOF){
		if(i >= maxLineLength){
			cout << "Input File too large!" << endl;
			return 0;
		}
		tmpCh = inputFile.get();
		line[i++] = tmpCh;
	}
	
	//s.copy(line, s.size(), 0);
	cout << "input size:" << strlen(line) << endl << endl;
	//cin >> line;
	cout << "-------------------input begin-------------------" << endl;
	cout << line << endl;
	cout << "--------------------input end--------------------" << endl;

	//cout << sy << endl;
	string str = "";
	
	// local test
	//addSubGrammarTable(level-1);
	
	// program test
	program();
	
	//VarState test
	//varState();
	
	//constState();
	//expression(str);
	//cout << "experssion:" << str << endl;
	//outputInfix();
	printInfixTable();
	//printIdTable();
	//printStaticIdTable();
	// infixNotaion optimization
	cout << "------------------After Optimiaztion---------------" << endl;
	optimizeInfixes();
	
	outputNewInfix();
	printNewInfixTable();
	printStaticIdTable();
	//return 0;
	//mips
	if(skipFlag){
		return 0;
	}
	mipsProgram();
	outputMipsCode("..\\GCC_MIPS_TEST\\MipsOutput_opt.s");
	system("pause");	
	return 0;
}
