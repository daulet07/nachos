#ifdef CHANGED
#include "frameProvider.h"
#include "strings.h"
#include "system.h"

FrameProvider::FrameProvider(){
	frame = new BitMap(NumPhysPages);
}

FrameProvider::~FrameProvider(){
	delete frame;
}

int FrameProvider::GetEmptyFrame(){
	int ret = frame->Find();

	if (ret != -1)
		bzero(&machine->mainMemory[ret*PageSize], PageSize);

	return ret;
}

void FrameProvider::ReleaseFrame(int pos){
	frame->Clear(pos);
}

int FrameProvider::NumAvailFrame(){
	return frame->NumClear();
}

#endif //CHANGED
