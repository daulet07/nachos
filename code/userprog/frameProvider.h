#ifdef CHANGED
#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H

#include "bitmap.h"

class FrameProvider{
	public:
		FrameProvider();
		~FrameProvider();
		int GetEmptyFrame();
		void ReleaseFrame(int pos);
		int NumAvailFrame();

	private:
		BitMap *frame;
};

#endif //FRAMEPROVIDER_H
#endif //CHANGED
