#include "syscall.h"

#define TRUE 1
#define FALSE 0

int strcmpstart(const char* c1, const  char* c2)
{

	while (*c2 != '\0' && *c1 == *c2 && *c1 != '\0')
	{
		c1 ++;
		c2 ++;
	}
	if (*c2 == '\0')
		return TRUE;
	else 
		return FALSE;
}


	int
main ()
{
	char read[100];
	char currentDir[100];
	currentDir[0] = '/';
	currentDir[1] = '\0';
	int finish = FALSE;
	while (!finish){
		PutString("<My Shell>");
		GetString(read, 100);

		if (strcmpstart(read, "exec"))
		{
			char *name = read;
			while (*name != '\0' && *name != ' ')
				name ++;

			if (*name == '\0')
				PutString("exec <program>\n");
			else
			{
				while (*name != '\0' && *name == ' ')
					name ++;

				if (*name == '\0')
					PutString("exec <program>\n");
				else
				{
					int processId = ForkExec(name);
					if (processId == -1)
						PutString("Error, the program not start\n");
					else
						Wait(processId);
					PutString("Program is finish\n");
				}
			}
			
		}
		else if (strcmpstart(read, "ls"))
			ListDir(currentDir);
		else if (strcmpstart(read, "mkdir"))
		{
			char *name = read;
			while (*name != '\0' && *name != ' ')
				name ++;

			if (*name == '\0')
				PutString("mkdir <fileName>\n");
			else
			{
				while (*name != '\0' && *name == ' ')
					name ++;

				if (*name == '\0' || *name == '/')
					PutString("mkdir <fileName>\n");

				int rest = Mkdir(currentDir, name);
				if (rest)
					PutString("directory create\n");
				else
					PutString("Error, the directory is not create\n");
			}
		}
		else if (strcmpstart(read, "exit"))
			finish = TRUE;
		else
			PutString("Commande not found\n");
		
	}
	PutString("Good by\n");
	return 0;
}
