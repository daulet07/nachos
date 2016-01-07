#include "syscall.h"

int main()
{
	int n;
	GetInt(&n);
	PutInt(n);
	PutString("\n");
	return 0;
}
