#include <iostream>
#include <cstdlib>
#include <cstdio>
#define STACK_SIZE 100
using namespace std;

int Action[12][6] = //shift >= 100, reduce 50~99, accept == -1, error 0~49
{
    105,   0,   0, 104,   0,   0,
      0, 106,   0,   0,   0,  -1,
      0,  52, 107,   0,  52,  52,
      0,  54,  54,   0,  54,  54,
    105,   0,   0, 104,   0,   0,
      0,  56,  56,   0,  56,  56,
    105,   0,   0, 104,   0,   0,
    105,   0,   0, 104,   0,   0,
      0, 106,   0,   0, 111,   0,
      0,  51, 107,   0,  51,  51,
      0,  53,  53,   0,  53,  53,
      0,  55,  55,   0,  55,  55
};


int Goto[12][3] = //Goto table, value are state number
{
    1, 2, 3,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    8, 2, 3,
    0, 0, 0,
    0, 9, 3,
    0, 0, 10,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0
};

char Grammar[20][10] = {'\0'};  //SLR1 grammar
char VT[10], VN[10]; //VT, VN
char AVT[6] = {'i', '+', '*', '(', ')', '$'}; //VT set
char GVN[3] = {'E', 'T', 'F'};  //VN set

int vnNum, vtNum, stateNum = 12;    //Number of VN, VT and state
int VNum[10]; //the length of each production in grammar
int grammarNum; //number of productions in grammar

typedef struct{
	char *base;
	char *top;
}SymbolStack;

typedef struct{
	int *base;
	int *top;
}StateStack;

SymbolStack symbol;
StateStack state;

int ScanGrammer(){
	FILE *fp = fopen("SLR_grammar.txt", "r");
	FILE *tp;
	char singleChar, nextChar;
	int i = 0, j = 0, count;
	while(!feof(fp)){
		fscanf(fp, "%c", &singleChar);
        if(singleChar == '?'){
            Grammar[i][j] = '\0';
            break;
        }
        else if(singleChar == '\n'){
            Grammar[i][j] = '\0';
            i++;
            j = 0;
            continue;
        }
        else if(singleChar == '-'){
            tp = fp;
            fscanf(tp, "%c", &nextChar);
            if(nextChar == '>'){
                fp = tp;
                continue;
            }
        }
        else if(singleChar == '|'){
            Grammar[i+1][0] = Grammar[i][0];
            Grammar[i][j] = '\0';
            i++;
            j = 1;
            continue;
        }
        Grammar[i][j] = singleChar;
        if(singleChar >= 'A' && singleChar <= 'Z'){ // Non-terminal Symbol
            count = 0;
            while(VN[count] != singleChar && VN[count] != '\0'){
                count++;
            }
            if(VN[count] == '\0'){
                if(singleChar == 'S'){
                    j++;
                    continue;
                }
                VN[count] = singleChar;
                vnNum = count + 1;
            }
        }
        else{       //Terminal Symbol
            count = 0;
            while(VT[count] != singleChar && VT[count] != '\0'){
                count++;
            }
            if(VT[count] == '\0'){
                VT[count] = singleChar;
                vtNum = count + 1;
            }
        }
        j++;
	}
    printf("The input grammar:\n");
    for(int k = 0; k <= i; k++){
        j = 0;
        while(Grammar[k][j] != '\0'){
            if(j == 1){
                cout << "->";
            }
            cout << Grammar[k][j];
            j++;
        }
        cout << endl;
    }
    count = 0;
    cout << "VT:";
    while(VT[count] != '\0'){
        cout << " " << VT[count++];
    }
    VT[count] = '$';
    vtNum = count + 1;
    cout << " " << VT[count] << endl;
    cout << "VN:";
    count = 0;
    while(VN[count] != '\0'){
        cout << " " << VN[count++];
    }
    cout << endl;
    cout << "vtNum:" << vtNum << " vnNum:" << vnNum << endl;
    fclose(fp);
    grammarNum = i + 1;
    return i;
}

void vNumcount(){
    cout << "vNumcount:";
    for(int i = 0; i < grammarNum; i++){
        int j = 0;
        while(Grammar[i][j] != '\0'){
            j++;
        }
        VNum[i] = j;
        cout << " " << VNum[i];
    }
    cout << endl;
    return ;
}

void InitStack(){
    state.base = (int *)malloc(STACK_SIZE*sizeof(int));
    if(!state.base)
        exit(1);
    state.top = state.base;
    *state.top = 0;
    symbol.base = (char *)malloc(STACK_SIZE*sizeof(char));
    if(!symbol.base)
        exit(1);
    symbol.top = symbol.base;
    *symbol.top = '$';
    return ;
}

int Judge(int statetop, char inputChar){
    // return value of action table
    int i, j;
    for(i = 0; i < stateNum; i++){
        if(statetop == i)
            break;
    }
    for(j = 0; j < vtNum; j++){
        if(inputChar == AVT[j])
            break;
    }
    return Action[i][j];
}

int GetGoto(int statetop, char inputChar){
    int i, j;
    for(i = 0; i < stateNum; i++){
        if(statetop == i)
            break;
    }
    for(j = 0; j < vnNum; j++){
        if(inputChar == GVN[j])
            break;
    }
    return Goto[i][j];
}

void print(int count, int i, char Input[], int action, int gt, int sign){
    /*
    count is step. i is current Input Str point.
    action, gt is current state value of action and goto.
    sign is the kind of operation.
    */
    //getchar();
    int *p = state.base, statetop;
    char *q = symbol.base, symboltop;
    // print step number
    cout << count << '\t';
    // print state stack
    while(p != state.top+1){
        statetop = *p++;
        cout << statetop;
    }
    cout << '\t';
    // print symbol stack
    while(q != symbol.top+1){
        symboltop = *q++;
        cout << symboltop;
    }
    cout << '\t';
    // print remainder input string
    int j = i, jj = 0;
    while(jj++ < j)
        cout << " ";
    while(Input[j] != '\0')
        cout << Input[j++];
    //print action and goto
    switch(sign){
        case 1: cout << "\ts" << action << "\t" << gt << endl;  break;
        case 2: cout << "\tr" << action << "\t" << gt << endl;  break;
        case 3: cout << "\tacc" << "\t" << gt << endl;  break;
        case 0: cout << "\t" << 0 << "\t" << 0 << endl;  break;
        default: break;
    }
    return;
}

int Pop(int action){
    int *p, statetop, ssValue, i;
    p = --state.top;
    statetop = *p;
    i = VNum[action] - 1; // gain production length
    while(i != 0){  //pop symbol stack top i
        symbol.top--;
        i--;
    }
    symbol.top++;
    *symbol.top = Grammar[action][0]; //push left VN of production
    ssValue = GetGoto(statetop, Grammar[action][0]); // Goto after reduction
    if(ssValue == 0)
        return ssValue;
    state.top++;
    *state.top = ssValue;
    return ssValue;
}

int Reduction(){
    char Input[20] = "i+i*i$";
    int i = 0, count = 1;
    int ssValue, action;
    int statetop, gt;
    int sign = -1; // shift==1, reduction == 2, accept == 3, error == -1;
    cout << "Please input the checked string:\n";
    cin >> Input;
    while(Input[i] != '\0'){
        if(Input[i] >= 'A' && Input[i] <= 'Z'){
            cout << "Invalid input, exit!\n";
            return 0;
        }
        i++;
    }
    i = 0;
    cout << "steps\tStStk\tSyStk\tInStr\tACTION\tGOTO" << endl;
    while(Input[i] != '\0'){
        if(count == 1){
            print(count, i, Input, 0, 0, 0);
            count++;
        }
        statetop = *state.top;
        ssValue = Judge(statetop, Input[i]);    //gain current action
        if(ssValue == 0){
            state.top--;
            if(*symbol.top == '$'){
                cout << "Reduction error!" << endl;
                return 0;
            }
            // error handling: Ignore Err
            continue;
        }
        if(ssValue == -1){
            sign = 3; // access
            print(count, i, Input, ssValue, 0, sign);
            count++;
            return 1;
        }
        if(ssValue >= 100){     // shift operation
            sign = 1;
            action = ssValue - 100;
            state.top++;
            *state.top = action;
            symbol.top++;
            *symbol.top = Input[i];
            i++;
            print(count, i, Input, action, 0, sign);
            count++;
        }
        if(ssValue >= 50 && ssValue < 100){ // reduce operation
            sign = 2;
            action = ssValue - 50;  //number of production for reducing
            gt = Pop(action);   // reduce
            print(count, i, Input, action, gt, sign);
            count++;
        }
    }
    return 0;
}

int main(){
    ScanGrammer();  //Input grammer
    vNumcount();    //Calculate the length of each production
    InitStack();    //Initilizate the state/symbol stack
    Reduction();    //Reduce the string checked
    system("pause");
	return 0;
}
