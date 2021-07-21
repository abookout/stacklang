#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INS_PUSH "push"
#define INS_POP "pop"
#define INS_ADD "add"
#define INS_IFEQ "ifeq"
#define INS_JUMP "jump"
#define INS_PRINT "print"
#define INS_DUP "dup"
#define INS_SWAP "swap"
#define INS_ROLL "roll"

#define bufSize 255
// need \n to keep it out of results of strtok
#define tokDelimiters " \t\n"

/*** Parsing & Execution Helpers ***/
void readLine();
void executeInstruction();
/*** End Parsing & Execution Helpers ***/


/*** Instruction Helpers ***/

// push a number onto the stack
void insPush(int val);

// pop the top number off the stack
int insPop();

// pop the top 2 numbers and push their sum onto the stack
void insAdd();

// if top of stack is 0 continue, else jump to <address> (a line number)
void insIfeq(size_t address);

// unconditional jump to the given address (a line number)
void insJump(size_t address);

// print the value at top of stack
void insPrint();

// push a copy of element on top of stack onto stack
void insDup();

// exchange the top two elements of the stack
void insSwap();

// cyclically permute all elements of the stack
void insRoll();

void swp(int* arr, size_t i, size_t j);
void die(int status);