extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <iostream>

#include "AVObject.h"

class ChromaFilter: public AVObject {

private:
	AVObject source;
	struct SwsContext *imgConvertCtx;

	public:

	ChromaFilter(AVObject);
	ChromaFilter(AVObject, int);

	void setAVSource (AVObject);

	
	void setAVChroma(PixelFormat);
	void setY4MChroma(int);

	int decodeNextFrame(void);
	void dumpFormat(void);
	
};

