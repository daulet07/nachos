#ifdef CHANGED
#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;

static Semaphore *semPut;
static Semaphore *semGet;

static Semaphore *semPutString;
static void ReadAvail(int arg) {readAvail->V();}
static void WriteDone(int arg) {writeDone->V();}

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
	semPut = new Semaphore("sem put", 1);
	semPutString = new Semaphore("sem put", 1);
	semGet = new Semaphore("sem get", 1);
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
	delete semPut;
	delete semPutString;
	delete semGet;
}

void SynchConsole::SynchPutChar(const char ch)
{
	semPut->P();
	console->PutChar(ch);
	writeDone->P();
	semPut->V();
}

char SynchConsole::SynchGetChar()
{
	semGet->P();
	readAvail->P();

	char ch;
	ch = console->GetChar();
	semGet->V();
	return ch;
}

void SynchConsole::SynchPutString(const char s[])
{
	while (*s != '\0')
	{
		SynchPutChar(*s);
		s ++;
	}
}
int SynchConsole::SynchGetString(char *s, int n)
{
	int i;
	for (i = 0; i < n; i ++)
	{
		s[i] = SynchGetChar();
		if (s[i] == '\n' || s[i] == EOF || s[i] == '\0')
		{
			i ++;
			break;
		}
	}
	
	return i;
}

void SynchConsole::SynchPutInt(int n)
{
	char *buffer = new char[MAX_STRING_SIZE];
	snprintf(buffer, MAX_STRING_SIZE, "%d", n);
	SynchPutString(buffer);
	delete []buffer;
}

void SynchConsole::SynchGetInt(int *n)
{
	char *buffer = new char[MAX_STRING_SIZE];
	SynchGetString(buffer, MAX_STRING_SIZE);
	if (sscanf(buffer, "%i", n) != 1)
		*n = 0;
	delete [] buffer;
}

#endif // CHANGED
