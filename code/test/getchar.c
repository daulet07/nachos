#include "syscall.h"
int main() {
	char c;
	while ((c = GetChar()) != 'q')
		PutChar(c);
	return 0;
}
