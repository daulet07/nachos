#include "syscall.h"

int main()
{
	char buffer[100];
	GetString(buffer, 8);
	PutString(buffer);
	PutString("\n");
	return 0;
}
