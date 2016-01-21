#include "reliable_post.h"
#include "system.h"

#define TEMP0 1000
#define MAXREEMISSIONS 5

static void TimerHandler(int arg)
{ ReliablePostOffice *rpo = (ReliablePostOffice *)arg; rpo->ReleaseLock(); }


ReliablePostOffice::ReliablePostOffice() : lock("ReliablePostOffice") {}


bool ReliablePostOffice::Send(PacketHeader pktHdr, MailHeader mailHdr, const char *data)
{
	PacketHeader inPktHdr;
	MailHeader inMailHdr;
	char buffer[MaxMailSize];

	for (int i = 0; i < MAXREEMISSIONS; i++)
	{
		printf("Length = %u", mailHdr.length);
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

void ReliablePostOffice::Receive(int box, PacketHeader *pktHdr, MailHeader *mailHdr, char* data)
{
	// Wait for the first message from the other machine
	postOffice->Receive(box, pktHdr, mailHdr, data);
	//printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
	//fflush(stdout);

	PacketHeader outPktHdr;
	MailHeader outMailHdr;
	const char *ack = "Got it!";
	// Send acknowledgement to the other machine (using "reply to" mailbox
	// in the message that just arrived
	outPktHdr.to = pktHdr->from;
	outMailHdr.to = mailHdr->from;
	outMailHdr.length = strlen(ack) + 1;
	postOffice->Send(outPktHdr, outMailHdr, ack);

	return true;
}

void ReliablePostOffice::ReleaseLock()
{
	lock.Release();
}
