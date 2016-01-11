#ifdef CHANGED
#include "system.h"
#include "thread.h"

extern int do_UserThreadCreate(int f, int arg);
void do_UserThreadExit();
void do_UserThreadJoin(int threadId);

typedef struct Argument{
	int f;
	int arg;
} Args;
#endif //CHANGED
