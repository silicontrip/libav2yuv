extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <iostream>

#include "AVException.h"
#include "AVObject.h"

class ChromaFilter: public AVObject {

private:
	AVObject * source;
	struct SwsContext *imgConvertCtx;

	public:

	ChromaFilter(AVObject*);
	ChromaFilter(AVObject*, int) throw (AVException*);

	~ChromaFilter();

	void setAVSource (AVObject*);

	// why aren't I overriding the AVObject methods setChromaSampling and setChromaSamplingFromY4M?
	void setAVChroma(PixelFormat) throw (AVException*);
	void setY4MChroma(int) throw (AVException*);

	void open(void);

	int decodeNextFrame(void);
	void dumpFormat(void);
	
};

