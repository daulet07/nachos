#include "syscall.h"

void print(void * arg) {
	int *id = (int*) arg;
	char text[] = "Hi ";
	PutString(text);
	PutInt(*id);
	PutChar('\n');

//	UserThreadExit();
}

int main() {

	PutString("Begin Main\n");

	int i;
	int nbThread = 10;
	int id[nbThread];
	for (i = 0; i < nbThread; i ++)
	{
		id[i] = i;
		if (UserThreadCreate(print, (void *)&id[i]) == -1)
		{
			PutString("Can't create thread ");
			PutInt(i);
			PutChar('\n');
		}

	}

	PutString("Ending Main\n");

	return 0;
}
