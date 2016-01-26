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

char *nextCommand(const char* com){
	char *c = (char*)com;
	while(*c == ' ' && *c != '\0')
		c ++;
	
	while (*c != ' ' && *c != '\0')
		c ++;

	while(*c == ' ' && *c != '\0')
		c ++;
	return c;
}

void strappend(char* to, char* from1, char* from2){
	while (*from1 != '\0')
	{
		*to = *from1;
		to ++;
		from1 ++;
	}
	while (*from2 != '\0')
	{
		*to = *from2;
		to ++;
		from2 ++;
	}
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
		PutString("<My Shell ");
		PutString(currentDir);
		PutString(" >");
		GetString(read, 100);
		char* com = read;

		com = nextCommand(com);
		if (strcmpstart(read, "exec"))
		{
			if (*com == '\0')
				PutString("exec <program>\n");
			else
			{
				int processId = ForkExec(com);
				if (processId == -1)
					PutString("Error, the program not start\n");
				else
					Wait(processId);
			}
		}
		else if (strcmpstart(read, "ls"))
		{
			if (*com == '\0')
				ListDir(currentDir);
			else
			{
				char path[100];
				strappend(path, currentDir, com);
				ListDir(path);
			}
		}
		else if (strcmpstart(read, "mkdir"))
		{
			if (*com == '\0')
				PutString("mkdir <dirName>\n");
			else
			{
				int rest = Mkdir(currentDir, com);
				if (!rest)
					PutString("Error, the directory is not create\n");
			}
		}
		else if (strcmpstart(read, "rmdir"))
		{
			if (*com == '\0')
				PutString("rmdir <dirName>\n");
			else
			{
				int rest = RmDir(currentDir, com);
				if (!rest)
					PutString("Error, the directory is not create delete. Maybe it is not empty\n");
			}
		}
		else if (strcmpstart(read, "create"))
		{
			if (*com == '\0')
				PutString("create <fileName>\n");
			else
			{
				FCreate(com);
				OpenFileId file = FOpen(com);
				if (file == -1)
				{
					PutString("Impossible to create file ");
					PutString(com);
					PutChar('\n');
				}
				else
				{
					PutString("File is create\n");
					FClose(file);
				}
			}
		}
		else if (strcmpstart(read, "write"))
		{
			if (*com == '\0')
				PutString("write <fileName>\n");
			else
			{
				OpenFileId file = FOpen(com);
				if (file == -1)
				{
					PutString("Impossible to open file ");
					PutString(com);
					PutChar('\n');
				}
				else
				{
					FWrite("HelloWorld\n", file);
					FClose(file);
				}
			}
		}
		else if (strcmpstart(read, "cat"))
		{
			if (*com == '\0')
				PutString("cat <fileName>\n");
			else
			{
				OpenFileId file = FOpen(com);
				if (file == -1)
				{
					PutString("Impossible to open file ");
					PutString(com);
					PutChar('\n');
				}
				else
				{
					char text[1000];
					FRead(text, 1000, file);
					PutString("Content of the file : \n");
					PutString(text);
					FClose(file);
				}
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
