#include "syscall.h"
#define MAX_THREADS 1000

int count=1;

void func(void *arg) {
	PutInt(count++);
	PutChar('\t');
}

int main() {

	int i;
	for (i=0; i<MAX_THREADS; i++)
	{
		int tid = UserThreadCreate(func, (void*)&i);
		if(tid==-1)
			PutString("\nThread Creation failed\n");
    		UserThreadJoin(tid);	
	}
	PutChar('\n');
    
	return 0;
}

