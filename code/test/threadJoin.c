#include "syscall.h"

void printNum(void * arg) {
	int i;
	for (i = 0; i < 7; i ++)
		PutInt(i);

	UserThreadExit();
}

void printAlpha(void *arg){
	char i;
	for (i = 'a'; i < 'l'; i ++)
		PutChar(i);
	UserThreadExit();
}

int main() {

	PutString("Begin Main\n");

	int threadId[2];
	threadId[0] = UserThreadCreate(printNum, 0);
	threadId[1] = UserThreadCreate(printAlpha, 0);

	UserThreadJoin(threadId[0]);	
	PutString("Poc\n");
	UserThreadJoin(threadId[1]);	
	
	PutString("Ending Main\n");


	return 0;
}
