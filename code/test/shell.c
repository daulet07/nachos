#include "syscall.h"

#define TRUE 1
#define FALSE 0

int strcmp(const char* c1, const  char* c2)
{
	while (*c2 != '\0' && *c1 == *c2 && *c1 != '\0')
	{
		c1 ++;
		c2 ++;
	}
	return *c1 == *c2;

}

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
	*to = *from2;
}

void strcpy(char *to, char* from){
	while (*from != '\0')
	{
		*to = *from;
		to ++;
		from ++;
	}
	*to = *from;
}

void getPath(char* to, char *path, char* name){
	if (*name == '\0')
	{
		strcpy(to, path);
		return;
	}

	while(*name == ' ')
		name ++;
	
	if (*name == '/')
	{
		strcpy(to, name);
		return;
	}

	strcpy(to, path);

	while(*name != '\0')
	{
		if (strcmpstart(name, ".."))
		{
			if (!strcmp(to, "/"))
			{
				char *tmp = to;
				while (*tmp != '\0')
					tmp++;
				while (*tmp != '/')
					tmp --;

				if (tmp == to)
					*(tmp+1) = '\0';
				else
					*tmp = '\0';
			}
		}
		else if (strcmpstart(name, "."));
		else
		{
			char *tmp = name;
			while (*tmp != '/' && *tmp != '\0')
				tmp ++;
			if (*tmp == '/')
			{
				*tmp = '\0';
				if (!strcmp(to, "/"))
					strappend(to, to, "/");
				strappend(to, to, name);
				*tmp = '/';
			}
			else
			{
				if (!strcmp(to, "/"))
					strappend(to, to, "/");
				strappend(to, to,  name);
			}
		}

		while (*name != '\0' && *name != '/')
			name ++;

		if (*name == '/')
			name ++;
	}
}

	int
main ()
{
	char read[100];
	char currentDir[100];
	strcpy(currentDir, "/");

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
				char path[100];
				getPath(path, currentDir, com);

				ProcessId processId = ForkExec(path);
				if (processId == -1)
					PutString("Program not start\n");
				else
					Wait(processId);
			}
		}
		else if (strcmpstart(read, "cd"))
		{
			if (*com == '\0')
				ListDir(currentDir);
			else
			{
				char path[100];
				getPath(path, currentDir, com);
				if (!IsDir(path))
				{
					PutString("Erreur, ");
					PutString(path);
					PutString(" is not a directory\n");
				}
				else
					strcpy(currentDir, path);
			}
		}
		else if (strcmpstart(read, "ls"))
		{
			if (*com == '\0')
				ListDir(currentDir);
			else
			{
				char path[100];
				getPath(path, currentDir, com);
				PutChar('\n');
				ListDir(path);
			}
		}
		else if (strcmpstart(read, "mkdir"))
		{
			if (*com == '\0')
				PutString("mkdir <dirName>\n");
			else
			{
				char path[100];
				getPath(path, currentDir, com);
				int rest = MkDir(path);
				if (!rest)
					PutString("Error, the directory is not create\n");
			}
		}
		else if (strcmpstart(read, "rm"))
		{
			if (*com == '\0')
				PutString("rm <fileName>\n");
			else
			{
				char path[100];
				getPath(path, currentDir, com);
				int result = Rm(path);
				if (!result)
					PutString("Error, the file is not delete\n");
			}
		}
		else if (strcmpstart(read, "rmdir"))
		{
			if (*com == '\0')
				PutString("rmdir <dirName>\n");
			else
			{
				char path[100];
				getPath(path, currentDir, com);
				int result = RmDir(path);
				if (!result)
					PutString("Error, the directory is not delete\n");
			}
		}
		else if (strcmpstart(read, "create"))
		{
			if (*com == '\0')
				PutString("create <fileName>\n");
			else
			{
				char path[100];
				getPath(path, currentDir, com);
				if (!FCreate(path))
				{
					PutString("Impossible to create file ");
					PutString(com);
					PutChar('\n');
				}
				else
					PutString("File is create\n");
			}
		}
		else if (strcmpstart(read, "write"))
		{
			if (*com == '\0')
				PutString("write <fileName>\n");
			else
			{
				char path[100];
				getPath(path, currentDir, com);
				OpenFileId file = FOpen(path);
				if (file == -1)
				{
					PutString("Impossible to open file ");
					PutString(com);
					PutChar('\n');
				}
				else
				{
					char tmp[10];
					while (FRead(tmp, 9, file) > 0);

					FWrite("Hello World\n", file);
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
				char path[100];
				getPath(path, currentDir, com);
				OpenFileId file = FOpen(path);
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
		
	}
	return 0;
}
