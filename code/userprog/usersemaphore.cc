#ifdef CHANGED

#include "usersemaphore.h"
#include "synch.h"

UserSemList::UserSemList() {
	first = NULL;
	last = NULL;
	semId = 0;
	lockId = new Lock("Lock User semaphore Id");
	lockPC = new Lock("Lock User semaphore Id");
}

UserSemList::~UserSemList() {
	delete lockId;
	delete lockPC;

	while(first != NULL) {
		delete first->sem;
		UserSem *temp = first;
		first = first->next;
		delete temp;
	}
}

int UserSemList::sem_create(int addr, int initialValue) {

	lockPC->Acquire();

	if(first==NULL) {
		first = new UserSem();
		last = first;
		first->prev = NULL;
		first->next = NULL;
	} else {
		last->next = new UserSem();
		last->next->prev = last;
		last = last->next;
	}

	last->id = assignId();
	int temp = last->id;
	last->sem = new Semaphore("User Semaphore",initialValue);

	lockPC->Release();

	return temp;
}

void UserSemList::sem_p(int id) {

	UserSem* semaphore = findUserSem(id);

	ASSERT(semaphore != NULL);
	if(semaphore)
	{

		semaphore->sem->P();
	}
}

void UserSemList::sem_v(int id) {

	UserSem* semaphore = findUserSem(id);
	ASSERT(semaphore != NULL);

	if(semaphore)
	{

		semaphore->sem->V();
	}
}

void UserSemList::sem_destroy(int id) {

	lockPC->Acquire();

	UserSem* semaphore = findUserSem(id);
	if(semaphore)
	{
		if(semaphore == first) {
			if(first!=last) {
				first = semaphore->next;
				first->prev = NULL;
			}
			else {
				first->prev = NULL;
				first->next = NULL;
			}
		} else if(semaphore == last ) {
			last = semaphore->prev;
			last->next = NULL;
		} else {
			semaphore->prev = semaphore->next;
			semaphore->next = semaphore->prev;
		}

		delete semaphore->sem;
		delete semaphore;
	}

	lockPC->Release();
}

int UserSemList::assignId()
{
	lockId->Acquire();
	int tempId = ++semId;
	lockId->Release();

	return tempId;
}

UserSem* UserSemList::findUserSem(int id) {

	UserSem* userSem = first;

	while(userSem != NULL && id > userSem->id) {
		userSem = userSem->next;
	}

	if(userSem != NULL && id == userSem->id) {
		return userSem;
	} else {
		return NULL;
	}
}

#endif // CHANGED
