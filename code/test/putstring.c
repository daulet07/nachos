#include "syscall.h"

int main() {
	
	char str[] = "Hello world!\n";
	int i;
	for (i = 0; i < 4; i ++)
		PutString(str);
	
	return 0;
}
