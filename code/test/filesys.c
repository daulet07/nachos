#include "syscall.h"

int main(){

	PutString("Hello World\n");
//	FCreate("/coucou");

	OpenFileId file = FOpen("/f1");
	if (file != -1)
	{
		PutString("File is open\n");
		char text[100];
		text[FRead(text, 100, file)] = '\0';
		PutString("Content of the file : \n");
		PutString(text);


		FWrite("File system is cool!!\n", 25, file);
		FClose(file);
		file = FOpen("/f1");
		char text2[100];
		PutString("And now: \n");
		text2[FRead(text2, 100, file)] = '\0';
		PutString(text2);
		PutString("Closing file\n");
		FClose(file);
	}
	else
		PutString("Error, the file is not create\n");

	return 0;
}
