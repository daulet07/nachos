#ifndef RELIABLE_POST_H
#define RELIABLE_POST_H

#include "post.h"
#include "synch.h"

class ReliablePostOffice
{
public:
	ReliablePostOffice();
    bool Send(PacketHeader pktHdr, MailHeader mailHdr, const char *data);
	void Receive(int box, PacketHeader *pktHdr, MailHeader *mailHdr, char *data);
	void ReleaseLock();
private:
	Lock lock;
};

#endif
