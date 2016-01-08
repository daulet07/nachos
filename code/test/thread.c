#include "syscall.h"

void print(void *arg) {
	arg = arg + 1;
	//int id = (int)arg;
//	PutString("I am thread ");
	//PutInt(id);
//	PutChar('\n');
	PutChar('a');

}

int main()
{
	UserThreadCreate(print, 0);
	/*
	PutString("Entring on the main\n");
	int tab[1], i;
	for (i = 0; i < 1; i ++)
	{
		PutString("Create thread ");
		PutInt(i);
		PutChar('\n');
		tab[i] = i;
		UserThreadCreate(print, &tab[i]);
	}
	PutString("Ending main\n");
	*/
	PutChar('b');
	PutChar('c');
	PutChar('d');
	PutChar('e');
	PutChar('f');
	PutChar('g');
	PutChar('h');
	PutChar('i');
	PutChar('j');
	PutChar('k');
	PutChar('l');

	return 0;
}
