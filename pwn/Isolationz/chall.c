#include "__malloc.h"

double resArr[10];
char* ptrArr[10];
int sizeArr[10];

void handler(){
    err("[!] Integer Overflow");
}

int isOperator(char ch)
{
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/');
}

int strReplace(char *string, char ch)
{
    int len = strlen(string);
    for (int i = 0; i < len; i++)
    {
        if(!isOperator(string[i]) && !isdigit(string[i]) 
        && string[i]!=' '){
            return -1;
        }

        if (string[i] == ch)
        {
            for (int j = i; j < len - 1; j++)
            {
                string[j] = string[j + 1];
            }
            i--;
            len--;
        }
    }
    string[len] = '.';
    string[len+1] = '\0';
    return 0;
}

// split the input into array of operators and operands
double evaluate(char *inp)
{
    int len = strlen(inp);
    if(len<=0){
        err("[!] strlen failed");
    }
    char num[len];
    int opLen = 0;
    int arrIdx = 0;
    int opArrIdx = 0;
    int flag = 0;
    int numIdx = 0;
    char prevOp = '\0';
    double res = 0;

    memset(num, '\x00', len);
    if(isOperator(inp[0]) || isOperator(inp[len-2])){
        err("[!] Invalid operator postion");
    }

    for (int i = 0; i < len; i++)
    {
        if(isOperator(inp[i]))
            opLen++;
    }

    // Initial operands and operators
    double arr[opLen+1];
    char opArr[opLen];

    // Operators and operands seperated
    for (int i = 0; i < len; i++)
    {
        // . is used to denote the end of input
        if (isOperator(inp[i]) || (inp[i]=='.' && i==len-1))
        {
            if(isOperator(inp[i-1]) && inp[i]!='.'){
                err("[!] Two operators together");
            }
            if(inp[i]=='.'){
                opArr[opArrIdx] = '\0';
                break;
            }
            // Let's do mul and div now itself
            if(inp[i]=='/' || inp[i]=='*'){
                flag = 1;
                prevOp = inp[i];
            }
            else{
                opArr[opArrIdx] = inp[i];
                opArrIdx++;
            }
        }
        else if(isdigit(inp[i])){
            num[numIdx++] = inp[i];
            if(!isdigit(inp[i+1])){
                arr[arrIdx] = atof(num);
                memset(num, '\0', 16);
                numIdx = 0;
                arrIdx++;
                // Div and Mul are done first
                if(flag){
                    if(prevOp=='/'){
                        arr[arrIdx-2] = arr[arrIdx-2] / arr[arrIdx-1];
                        arrIdx--;
                    }
                    else{
                        arr[arrIdx-2] = arr[arrIdx-2] * arr[arrIdx-1];
                        arrIdx--;
                    }
                }
                flag = 0;
            }
        }
        // Only numbers and + - / * allowed
        else
        {
            err("[!] Invalid character");
        }
    }
    res = arr[0];
    for(int i=0; i<strlen(opArr); i++){
        switch (opArr[i])
        {
        case '+': res+= arr[i+1];
            break;
        case '-': res-= arr[i+1];
            break;
        default:
            break;
        }
    }
    return res;
}

void getInp(char* buf, int len){
    int ret = read(0, buf, len-1);
    if(ret<=0){
        err("[!] Read failed");
    }
    buf[ret] = '\0';
}

unsigned int getInt(){
    char buf[32];
    getInp(buf, 32);
    return atoi(buf);
}

int menu(){
    log("\n*************************************\n");
    log("|1. Add Calculation                 |\n");
    log("|2. Delete Calculation              |\n");
    log("|3. List Calculation                |\n");
    log("|4. Exit                            |\n");
    log("*************************************\n");
    log(">> ");
    return getInt();
}

void addCalc(){
    unsigned int size = 0;
    char* ptr = NULL;
    double res = 0;
    unsigned int idx = 0;
    int len = 0;
    int ret = 0;

    for(idx=0; idx<10; idx++){
        if(!ptrArr[idx])
            break;
    }

    if(idx>=10){
        err("[!] No more space in array");
    }

    log("Enter size: ");
    size = getInt();
    
    // __malloc takes care of upper limit
    if(size<=0){
        err("[!] Invalid size");
    }

    log("Enter expression: ");
    ptr = __malloc(size);
    getInp(ptr, size);
    len = strlen(ptr);
    if(len<=0){
        err("[!] strlen failed");
    }
    // We don't want to modify user input
    char inp[len+2];
    strncpy(inp, ptr, len);
    if(inp[len-1]=='\n'){
        ptr[len-1] = '\0';
        inp[len-1] = '\0';
    }
    else
        inp[len] = '\0';
    ret = strReplace(inp, ' ');
    if(ret<0){
        err("[!] Invalid character");
    }

    res = evaluate(inp);
    resArr[idx] = res;
    sizeArr[idx] = size;
    ptrArr[idx] = ptr;
    log("%s = %.2f\n", ptrArr[idx], resArr[idx]);
}

void delCalc(){
    unsigned int idx = 0;
    log("Enter index: ");
    idx = getInt();
    if(idx>=10){
        err("[!] Invalid index");
    }
    if(!ptrArr[idx]){
        err("[!] Calc doesn't exist");
    }
    __free(ptrArr[idx]);
    ptrArr[idx] = NULL;
    sizeArr[idx] = 0;
    resArr[idx] = 0;
}

void listCalc(){
    for(int i=0; i<10; i++){
        if(ptrArr[i]){
                log("%s = %.2f\n", ptrArr[i], resArr[i]);
        }
    }
}

int main()
{
    setvbuf(stdin, 0, _IONBF, 0);
    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);

    signal(SIGFPE, handler);

    while(1){
        switch (menu())
        {
        case 1: addCalc();
            break;
        case 2: delCalc();
            break;
        case 3: listCalc();
            break;
        case 4: err("Bye");
        default: puts("Invalid");
        }
    }
}