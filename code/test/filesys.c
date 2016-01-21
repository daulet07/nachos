#include "syscall.h"

int main(){

	PutString("Hello World\n");
//	FCreate("/coucou");

	OpenFileId file = FOpen("/coucou");
	if (file != -1)
	{
		PutString("File is open\n");
		char text[100];
		FRead(text, 100, file);
		PutString("Content of the file : \n");
		PutString(text);
		
		FWrite("File system is boring!!\n", 24, file);
		FRead(text, 100, file);
		PutString("And now: \n");
		PutString(text);
		PutString("Closing file\n");
		FClose(file);
	}
	else
		PutString("Error, the file is not create\n");

	return 0;
}
