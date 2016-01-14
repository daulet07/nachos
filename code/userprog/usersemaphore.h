#ifdef CHANGED
#ifndef USERSEMAPHORE_H
#define USERSEMAPHORE_H

class Semaphore;
class Lock;
struct UserSem {
	Semaphore* sem;
	int id;
	struct UserSem *next;
	struct UserSem *prev;
};
typedef struct UserSem UserSem;

class UserSemList
{
public:
	UserSemList();
	~UserSemList();
	int sem_create(int addr, int initialValue);
	void sem_p(int id);
	void sem_v(int id);
	void sem_destroy(int id);

private:

	UserSem *first;
	UserSem *last;
	int semId;
	Lock *lockPC;
	Lock *lockId;

	int assignId();
	UserSem* findUserSem(int id);
};
#endif // USERSEMAPHORE_H
#endif // CHANGED
