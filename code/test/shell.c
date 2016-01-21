#include "syscall.h"

/*
bool strcmp(const char* c1,const  char* c2)
{
	char* t1 = c1;
	char* t2 = c2;

	while (*t2 != '\0' && *t1 == *t2 && c2 != '\0')
	{
		t1 ++;
		t2 ++;
	}

	return t2 == '\0';
}
*/

	int
main ()
{
	SpaceId newProc;
	OpenFileId input = ConsoleInput;
	OpenFileId output = ConsoleOutput;
	char prompt[2], buffer[60];
	int i;

	prompt[0] = '-';
	prompt[1] = '-';

	while (1)
	{
		Write (prompt, 2, output);

		i = 0;

		do
		{

			Read (&buffer[i], 1, input);

		}
		while (buffer[i++] != '\n');

		buffer[--i] = '\0';

		if (i > 0)
		{
			newProc = Exec (buffer);
			Join (newProc);
		}
	}
	/*

	char read[100];
	char *
	while (true){
		GetString(read, 100);

		if (strcmp(read, "ls
		
	}
	*/
}
