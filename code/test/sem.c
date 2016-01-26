#include "syscall.h"

#define MAX_THREADS 5

sem_t mutex;

int C = 0;

void safe_increment(void* arg) {
	P(mutex);

	int temp = C;
	PutString("I am new thread");
	PutChar('\n');
	C = temp + 1;

	PutString("New value : ");
	PutInt(C);
	PutChar('\n');
	V(mutex);

	UserThreadExit();
}


int main() {

	int tid[MAX_THREADS], i;
	PutString("At the beginning\n");
	mutex = SemInit(1);

	for (i = 0; i < MAX_THREADS; ++i){
		tid[i] = UserThreadCreate(safe_increment, 0);
	}

	for (i = 0; i < MAX_THREADS; ++i){
		UserThreadJoin(tid[i]);
	}

	SemDestroy(mutex);
	PutString("At the end\n");

	return 0;
}
