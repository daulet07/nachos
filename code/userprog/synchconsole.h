#ifdef CHANGED
#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H
#include "copyright.h"
#include "utility.h"
#include "console.h"
#include "synch.h"

class SynchConsole {
	public:
		SynchConsole(char *readFile, char *writeFile);
		// initialize the hardware console device
		~SynchConsole(); // clean up console emulation
		void SynchPutChar(const char ch); // Unix putchar(3S)
		char SynchGetChar(); // Unix getchar(3S)
		void SynchPutString(const char *s); // Unix puts(3S)
		int SynchGetString(char *s, int n); // Unix fgets(3S)
		void SynchPutInt(int n); // Unix snprintf(3S)
		void SynchGetInt(int *n); // Unix sscanf(3S)
	private:
		Console *console;

		Lock* lockPut;
		Lock* lockGet;
};
#endif // SYNCHCONSOLE_H
#endif // CHANGED
