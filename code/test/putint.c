#include "syscall.h"

int main() {
	int i;
	for (i = 0; i < 10; i ++)
		PutChar('a'+i);
	
	PutChar('\n');
	return 0;
}
