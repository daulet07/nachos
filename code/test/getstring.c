#include "syscall.h"

int main()
{
	char buffer[100];
	GetString(buffer, 5);
	PutString(buffer);
	Halt();
	return 0;
}
