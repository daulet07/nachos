#ifndef VECTOR_H
#define VECTOR_H

#ifdef CHANGED
#include "list.h"

class Lock;
class ListWaiter
{
	public:
		ListWaiter();
		~ListWaiter();

		void New(unsigned int id);
		void Wait(unsigned int id);
		void End(unsigned int id);
	
	private:
		Lock *lock;
		
		struct ElemWaiter{
			struct ElemWaiter *next;
			unsigned int id;
			List *waiter;
		};
		typedef struct ElemWaiter ElemWaiter;
		
		ElemWaiter *first;
		ElemWaiter *last;
};

#endif //CHANGED
#endif //VECTOR_H
