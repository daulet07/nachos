#include "reliable_post.h"
#include "system.h"

#define TEMP0 1000
#define MAXREEMISSIONS 5

static void TimerHandler(int arg)
{ ReliablePostOffice *rpo = (ReliablePostOffice *)arg; rpo->ReleaseLock(); }


ReliablePostOffice::ReliablePostOffice() : lock("ReliablePostOffice")
{
}


bool ReliablePostOffice::Send(PacketHeader pktHdr, MailHeader mailHdr, const char *data)
{
	PacketHeader inPktHdr;
	MailHeader inMailHdr;
	char buffer[MaxMailSize];

	for (int i = 0; i < MAXREEMISSIONS; i++)
	{
		postOffice->Send(pktHdr, mailHdr, data);
		interrupt->Schedule(TimerHandler, (int)this, TEMP0, NetworkRecvInt);
		lock.Acquire();

		if (!postOffice->IsMailBoxEmpty(1))
		{
			postOffice->Receive(1, &inPktHdr, &inMailHdr, buffer);
			return true;
		}
	}
	return false;
}

bool ReliablePostOffice::Receive()
{
	return true;
}

void ReliablePostOffice::ReleaseLock()
{
	lock.Release();
}
