#include "syscall.h"

#define MAX_THREADS 8
int count=0;
sem_t mutex;

int C = 0;

void safe_increment(void* arg) {
	P(mutex);

	int temp = C;
	//PutChar('\t');
	C = temp + 1;

	//PutString("My ID is--- : ");
	PutInt(C);
	PutInt(C);
	PutInt(C);
	PutChar('\t');
	V(mutex);

	//UserThreadExit();
}


int main() {

	int tid[MAX_THREADS], i;
	//PutString("\nAt the beginning of p3\n");
	mutex = SemInit(1);

	for (i = 0; i < MAX_THREADS; ++i){
		tid[i] = UserThreadCreate(safe_increment, 0);
		count++;
	}

	for (i = 0; i < MAX_THREADS; ++i){
		UserThreadJoin(tid[i]);
	}

	SemDestroy(mutex);
	//PutString("At the end\n");

	return 0;
}
