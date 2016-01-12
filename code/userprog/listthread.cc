#ifdef CHANGED
#include "listthread.h"
#include "synch.h"

ListThread::ListThread(){
	first = NULL;
	last = NULL;
	lock = new Lock("lock for listThread");
}

ListThread::~ListThread(){
	while (first != NULL)
	{
		ElemThread *next = first->next;
		delete first->waiter;
		delete first;
		first = next;
	}

	delete lock;
}

void ListThread::newThread(unsigned int id){
	lock->Acquire();
	if (first == NULL)
	{
		first = new ElemThread;
		last = first;
	}
	else 
	{
		last->next = new ElemThread;
		last = last->next;
	}

	last->next = NULL;
	last->id = id;
	last->waiter = new List();
	lock->Release();
}

void ListThread::waitThread(unsigned int id){
	if (first == NULL)
		return;

	ElemThread * thread = first;
	while (thread != NULL && thread->id < id)
		thread = thread->next;

	if (thread->id != id)
		return;

	Lock *lockTmp = new Lock("Lock for thread");
	thread->waiter->Append(lockTmp);
	lockTmp->Acquire();
	lockTmp->Acquire();

	delete lockTmp;
}

void ListThread::endThread(unsigned int id){
	lock->Acquire();
	ElemThread *thread = first;
	ASSERT(thread != NULL);

	if (first->id == id)
	{
		first = first->next;
	}
	else
	{
		ElemThread *prev = first;
		thread = first->next;

		while (thread != NULL && thread->id < id)
		{
			prev = thread;
			thread = thread->next;
		}
		ASSERT(thread != NULL);
		ASSERT(thread->id == id);

		if (thread == last)
			last = prev;
		prev->next = thread->next;
	}
	lock->Release();

	Lock *l;
	while ((l = (Lock*)thread->waiter->Remove()) != NULL)
		l->Release();

	delete thread->waiter;
	delete thread;
}

#endif //CHANGED
