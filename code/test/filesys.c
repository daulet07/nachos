#include "syscall.h"

int main(){

	PutString("Hello World\n");
	FCreate("/f1");

	OpenFileId file = FOpen("/f1");
	if (file != -1)
	{
		PutString("File is open\n");
		char text[1000];
		FRead(text, 1000, file);
		PutString("Content of the file : \n");
		PutString(text);


		FWrite("COUcou pocpoc\n", file);
		FClose(file);
		file = FOpen("/f1");
		char text2[1000];
		PutString("And now: \n");
		FRead(text2, 1000, file);
		PutString(text2);
		PutString("Closing file\n");
		FClose(file);
	}
	else
		PutString("Error, the file is not create\n");

	return 0;
}
