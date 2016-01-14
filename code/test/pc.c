#include "syscall.h"

#define BUFFER_SIZE 3

int buff[BUFFER_SIZE];

sem_t mutex;
sem_t full;
sem_t empty;

int head = 0, tail = 0;

int C = 0;

int produce_item() {
	PutString("Producing: ");
	PutInt(C);
	PutChar('\n');

	return C++;
}

void consume_item(int n) {
	PutString("Consuming: ");
	PutInt(n);
	PutChar('\n');
}

void push(int n) {
	buff[tail] = n;
	tail = (tail + 1) % BUFFER_SIZE;
}

int pop() {
	int v = buff[head];
	head = (head + 1) % BUFFER_SIZE;
	return v;
}

void producer(void* arg) {
	int i;
	for (i = 0 ; i< 100; i++) {
		P(empty);
			P(mutex);
			int item = produce_item();
			push(item);
			V(mutex);
		V(full);
	}

	UserThreadExit();
}

void consumer(void* arg) {
//	while (1) {
	int i;
	for ( i = 0 ; i< 100; i++) {
		P(full);
			P(mutex);
			int item = pop();
			consume_item(item);

			V(mutex);
		V(empty);
	}


	UserThreadExit();
}

int main() {

	int pid_t1, pid_t2;

	mutex = SemInit(mutex, 1);
	full = SemInit(full, 0);
	empty = SemInit(empty, BUFFER_SIZE);
	PutString("=== ");
	PutInt(mutex);
	PutInt(full);
	PutInt(empty);


	pid_t1 = UserThreadCreate(producer, 0);
	pid_t2 = UserThreadCreate(consumer, 0);

	UserThreadJoin(pid_t1);
	UserThreadJoin(pid_t2);

//	UserThreadExit();

	return 0;
}
