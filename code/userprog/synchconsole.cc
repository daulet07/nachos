#ifdef CHANGED
#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"
static Semaphore *readAvail;
static Semaphore *writeDone;
static void ReadAvail(int arg) {readAvail->V(); }
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
	readAvail->P();
	char ch;
	ch = console->GetChar();
	return ch;
}
void SynchConsole::SynchPutString(const char s[])
{
	writeDone->P();
	while (*s != '\0')
	{
		SynchPutChar(*s);
		s ++;
	}
	writeDone->V();
}
void SynchConsole::SynchGetString(char *s, int n)
{
	readAvail->P();
	char c = SynchGetChar();
	while (c != EOF && c!= '\0' && n > 0)
	{
		*s = c;
		s ++;
		c = SynchGetChar();
	}
	*s = '\0';
	readAvail->V();
}
#endif // CHANGED
