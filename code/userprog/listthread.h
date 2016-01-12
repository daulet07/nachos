#ifndef VECTOR_H
#define VECTOR_H

#ifdef CHANGED
#include "list.h"

class Lock;
class ListThread
{
	public:
		ListThread();
		~ListThread();

		void newThread(unsigned int id);
		void waitThread(unsigned int id);
		void endThread(unsigned int id);
	
	private:
		Lock *lock;
		
		struct ElemThread{
			struct ElemThread *next;
			unsigned int id;
			List *waiter;
		};
		typedef struct ElemThread ElemThread;
		
		ElemThread *first;
		ElemThread *last;
};

#endif //CHANGED
#endif //VECTOR_H
