#include "myMain.h"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char** argv) {
	string s = "(9+9*9)*9";
	s.copy(line, s.size(), 0);
	//cin >> line;
	cout << line << endl;
	nextchar();
	string str = "";
	E(str);
	cout << "\nSucceed!" << endl;
	outputInfix();
	printInfixTable();
	system("pause");	
	return 0;
}
