#ifdef CHANGED
#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;

static void ReadAvail(int arg) {readAvail->V();}
static void WriteDone(int arg) {writeDone->V();}

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
	lockPut = new Lock("lock put");
	lockGet = new Lock("lock put");
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
	delete lockPut;
	delete lockGet;
}

void SynchConsole::SynchPutChar(const char ch)
{
	lockPut->Acquire();
	console->PutChar(ch);
	writeDone->P();
	lockPut->Release();
}

char SynchConsole::SynchGetChar()
{
	lockGet->Acquire();
	readAvail->P();

	char ch;
	ch = console->GetChar();
	lockGet->Release();
	return ch;
}

void SynchConsole::SynchPutString(const char s[], int length)
{
	for (int i = 0; i < length; i++, s++)
		SynchPutChar(*s);
}

void SynchConsole::SynchPutString(const char s[])
{
	while (*s != '\0')
	{
		SynchPutChar(*s);
		s++;
	}
}

int SynchConsole::SynchGetString(char *s, int n)
{
	int i = 0;
	do {
		s[i] = SynchGetChar();
		i ++;
	} while (i < n && s[i-1] != '\0' && s[i-1] != '\n');
	return i;

	for (i = 0; i < n; i++)
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
	delete[] buffer;
}

void SynchConsole::SynchGetInt(int *n)
{
	char *buffer = new char[MAX_STRING_SIZE];
	SynchGetString(buffer, MAX_STRING_SIZE);
	if (sscanf(buffer, "%i", n) != 1)
		*n = 0;
	delete[] buffer;
}

#endif // CHANGED
