#include "syscall.h"

void print(void * arg) {

	int *num = (int*)arg;

	PutString("Printing from user thread print!");
	PutInt(*num);
	PutChar('\n');

	UserThreadExit();

}

int main() {

	PutString("Begin Main");

	int i =10;
	UserThreadCreate(print, (void *) &i);

	PutChar('\n');
	PutString("Ending Main");

	return 0;
}
