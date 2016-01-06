#ifdef CHANGED
#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;

static void ReadAvail(int arg) { readAvail->V();}
static void WriteDone(int arg) {writeDone->V(); }

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch)
{
	console->PutChar(ch);
	writeDone->P();
}

char SynchConsole::SynchGetChar()
{
//	fprintf(stderr, "Synchconsole.cc Whant getChar\n");
	readAvail->P();
	char ch;
	ch = console->GetChar();
//	fprintf(stderr, "Synchconsole.cc take char %c\n", ch);
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

void SynchConsole::SynchGetString(char *s, int n)
{
	int i;
	for (i = 0; i < n-1; i ++)
	{
		s[i] = SynchGetChar();
		if (s[i] == '\n')
			break;
	}
	s[i] = '\0';

/*
	char c = SynchGetChar();
	while (c != EOF && c!= '\0' && n > 0)
	{
		*s = c;
		fprintf(stderr, "- %c\n", *s);
		s ++;
		c = SynchGetChar();
		n --;
		
	}
	*/
}

#endif // CHANGED
