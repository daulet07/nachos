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
	pthread_mutex_init(&mutex, NULL);
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
	pthread_mutex_destroy(&mutex);
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

int SynchConsole::SynchGetString(char *s, int n)
{
	pthread_mutex_lock(&mutex);
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
	
	pthread_mutex_unlock(&mutex);
	return i;

/*
	s[i] = '\0';
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
