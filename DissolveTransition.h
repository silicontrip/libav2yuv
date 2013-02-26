extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <iostream>

#include "AVObject.h"

class DissolveTransition: public AVObject {
	
	
	AVObject * fromAV;
	AVObject * toAV;

	int frameCounter;
	
	unsigned int frames;
	
public:
	DissolveTransition(AVObject *, AVObject *, unsigned int);
	~DissolveTransition();
	
	void setAVFrom (AVObject *);
	void setAVTo (AVObject *);
	
	void setFrames (unsigned int);
	
	int decodeNextFrame(void);
	void dumpFormat(void);
	
	
};