#include "syscall.h"

void print(void * arg) {
	int *id = (int*) arg;
	char text[] = "Hi ";
	PutString(text);
	PutInt(*id);
	PutChar('\n');

	UserThreadExit();
}

int main() {

	PutString("Begin Main\n");

	int i =10;
	int id[5];
	for (i = 0; i < 5; i ++)
	{
		id[i] = i;
		UserThreadCreate(print, (void *)&id[i]);
	}

	PutString("Ending Main\n");

	return 0;
}
