#include "syscall.h"

int main(){

	PutString("Hello World\n");
	FCreate("/coucou");

	OpenFileId file = FOpen("/coucou");
	if (file != -1)
	{
		PutString("File is create\n");
		char text[100];
		FRead(text, 100, file);
		PutString(text);
		PutString("Closing file\n");
		FClose(file);
	}
	else
		PutString("Error, the file is not create\n");

	return 0;
}
