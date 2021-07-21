#include "stacklang.h"

/*
    Directly inspired from https://stackoverflow.com/a/6888074
    This language consists of a stack and 9 instructions:
    - push <num>: push a number onto the stack
    - pop: pop the top number off the stack
    - add: pop the top 2 numbers and push their sum onto the stack
    - ifeq <address>: if top of stack is 0 continue, else jump to <address> which
    is a line number
    - jump <address>: unconditional jump to a line number
    - print: print the value at top of stack
    - dup: push a copy of top of stack onto stack
    - swap: exchange the top two elements of the stack
    - roll: cyclically permute all elements in the stack, so e.g. A B C
    becomes B C A

    Syntax rules:
    - Each instruction must be on its own line
    - Instructions must be separated from their parameter with only spaces or
    tabs (where applicable)
    - Every line must be either entirely whitespace, start with an instruction,
    or start with a '#' for a comment.
    - A line may have a comment starting with a '#' after the instruction
*/

size_t stackSize;       // number of elements in the stack
size_t stackCapacity;   // number of elements the stack can currently hold
int* stack;

// buffer to hold single line from program file
char fileBuf[bufSize];

// current line number of program file
size_t programLine = 1;
// file pointer for program file
FILE* fp;

int main(int argc, char* argv[]){
    if (argc != 2){
        fprintf(stderr, "Bad arguments.\n");
        return 1;
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL){
        fprintf(stderr, "File could not be read: '%s'\n", argv[1]);
        return 1;
    }


    // Create stack with a default size of 8
    stackSize = 0;
    stackCapacity = 8;
    stack = (int*) malloc(stackCapacity * sizeof(int));

    while (1){
        // read a line from the program file
        readLine();

        // execute an instruction based on the line read
        executeInstruction();
    }

    die(0);
}

/*** Parsing & Execution Helpers ***/

// Read line 'programLine' from the program file and store it in fileBuf.
void readLine(){
    size_t count = 0;
    fseek(fp, 0, SEEK_SET);
    while (fgets(fileBuf, bufSize, fp)){
        count++;
        if (count == programLine) {
            int lineLen = strcspn(fileBuf, "\n"); // don't print \n
            printf("\t%ld: '%.*s'\n", programLine, lineLen, fileBuf);
            return;
        }
    }

    if (feof(fp)){
        printf("Reached end of file.");
        die(0);
    }

    fprintf(stderr, "Could not read line %ld from program with %ld lines.",
        programLine, count);
    die(1);
}

void executeInstruction(){
    // parse contents of fileBuf, trimming whitespace, ignoring comments, and
    //  figuring out which instruction to run

    char* tok = strtok(fileBuf, tokDelimiters);

    if (tok == NULL || tok[0] == '#'){
        // only found whitespace or a comment
        programLine++;
        return;
    } else if (strcmp(tok, INS_PUSH) == 0){
        tok = strtok(NULL, tokDelimiters);
        if (tok == NULL || tok[0] == '#'){
            fprintf(stderr, "Line %ld: push requires a parameter", programLine);
            die(1);
        }
        int num = strtol(tok, NULL, 10);
        insPush(num);
        programLine++;
    } else if (strcmp(tok, INS_POP) == 0){
        insPop();
        programLine++;
    } else if (strcmp(tok, INS_ADD) == 0){
        insAdd();
        programLine++;
    } else if (strcmp(tok, INS_IFEQ) == 0){
        tok = strtok(NULL, tokDelimiters);
        if (tok == NULL || tok[0] == '#'){
            fprintf(stderr, "Line %ld: ifeq requires a parameter", programLine);
            die(1);
        }
        size_t addr = strtol(tok, NULL, 10);
        insIfeq(addr);
    } else if (strcmp(tok, INS_JUMP) == 0){
        tok = strtok(NULL, tokDelimiters);
        size_t addr = strtol(tok, NULL, 10);
        insJump(addr);
    } else if (strcmp(tok, INS_PRINT) == 0){
        insPrint();
        programLine++;
    } else if (strcmp(tok, INS_DUP) == 0){
        insDup();
        programLine++;
    } else if (strcmp(tok, INS_SWAP) == 0){
        insSwap();
        programLine++;
    } else if (strcmp(tok, INS_ROLL) == 0){
        insRoll();
        programLine++;
    } else {
        fprintf(stderr, "Line %ld: Unexpected token '%s'", programLine, tok);
        die(1);
    }

}

/*** End Parsing & Execution Helpers ***/


/*** Instruction Helpers ***/

// push a number onto the stack
void insPush(int val){
    if (stackSize == stackCapacity){
        printf("insPush resizing size %ld, capacity %ld\n", stackSize, stackCapacity);
        stackCapacity *= 2;
        stack = (int*) realloc(stack, stackCapacity * sizeof(int));
    }
    
    stack[stackSize] = val;
    stackSize++;
}

// pop the top number off the stack
int insPop(){
    // don't bother shrinking stack
    if (stackSize == 0){
        fprintf(stderr, "Line %ld: Tried to pop when stack has 0 elements",
            programLine);
        die(1);
    }

    int num = stack[stackSize-1];
    stack[stackSize-1] = 0;
    stackSize--;

    return num;
}

// pop the top 2 numbers and push their sum onto the stack
void insAdd(){
    if (stackSize < 2){
        fprintf(stderr, "Line %ld: Tried to add when stack has %ld elements",
            programLine, stackSize);
        die(1);
    }
    int a = insPop();
    int b = insPop();
    insPush(a + b);
}

// if top of stack is 0 continue, else jump to <address> (a line number)
void insIfeq(size_t address){
    if (stack[stackSize-1] != 0){
        insJump(address);
    } else {
        programLine++;
    }
}

// unconditional jump to the given address (a line number)
void insJump(size_t address){
    programLine = address;
}   

// print the value at top of stack
void insPrint(){
    printf("%d\n", stack[stackSize-1]);
}

// push a copy of element on top of stack onto stack
void insDup(){
    if (stackSize == 0){
        fprintf(stderr, "Line %ld: Tried to dup when stack has 0 elements",
            programLine);
        die(1);
    }
    insPush(stack[stackSize-1]);
}

// exchange the top two elements of the stack
void insSwap(){
    if (stackSize < 2){
        fprintf(stderr, "Line %ld: Tried to swap when stack has %ld elements",
            programLine, stackSize);
        die(1);
    }

    swp(stack, stackSize - 2, stackSize - 1);
}

// cyclically permute all elements of the stack
void insRoll(){
    int tmp = stack[stackSize-1];
    for (int i = stackSize-1; i > 0; i--){
        stack[i] = stack[i-1];
    }
    stack[0] = tmp;
}

/*** End Instruction Helpers ***/

void swp(int* arr, size_t i, size_t j){
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

void die(int status){
    printf("\n");
    fclose(fp);
    free(stack);
    exit(status);
}