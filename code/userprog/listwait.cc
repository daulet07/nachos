#ifdef CHANGED
#include "listwait.h"
#include "synch.h"

ListWaiter::ListWaiter(){
	first = NULL;
	last = NULL;
	lock = new Lock("lock for listWaiter");
}

ListWaiter::~ListWaiter(){
	while (first != NULL)
	{
		ElemWaiter *next = first->next;
		delete first->waiter;
		delete first;
		first = next;
	}

	delete lock;
}

void ListWaiter::New(unsigned int id){
	lock->Acquire();
	if (first == NULL)
	{
		first = new ElemWaiter;
		last = first;
	}
	else 
	{
		last->next = new ElemWaiter;
		last = last->next;
	}

	last->next = NULL;
	last->id = id;
	last->waiter = new List();
	lock->Release();
}

void ListWaiter::Wait(unsigned int id){
	if (first == NULL)
		return;

	ElemWaiter * elem = first;
	while (elem != NULL && elem->id < id)
		elem = elem->next;


	if (elem == NULL || elem->id != id)
		return;

	Lock *lockTmp = new Lock("Lock for elem");
	elem->waiter->Append(lockTmp);
	lockTmp->Acquire();
	lockTmp->Acquire();

	delete lockTmp;
}

void ListWaiter::End(unsigned int id){
	if (first == NULL)
		return;
	lock->Acquire();
	ElemWaiter *elem = first;

	if (first->id == id)
	{
		first = first->next;
	}
	else
	{
		ElemWaiter *prev = first;
		elem = first->next;

		while (elem != NULL && elem->id < id)
		{
			prev = elem;
			elem = elem->next;
		}
		if (elem == NULL || elem->id != id)
		{
			lock->Release();
			return;
		}
		ASSERT(elem != NULL);
		ASSERT(elem->id == id);

		if (elem == last)
			last = prev;
		prev->next = elem->next;
	}
	lock->Release();

	Lock *l;
	while ((l = (Lock*)elem->waiter->Remove()) != NULL)
		l->Release();

	delete elem->waiter;
	delete elem;
}

#endif //CHANGED
