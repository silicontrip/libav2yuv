extern "C" {
#include <yuv4mpeg.h>
#include <mpegconsts.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <fcntl.h>
}

#include <string>
#include <iostream>
#include "AVException.h"

class Libyuv {

protected:
	y4m_stream_info_t yuvStreamInfo;
	y4m_frame_info_t yuvFrameInfo;
	uint8_t **frameData;
	int fdOut;
	bool fileHeaderWritten;

	void init(void) throw (AVException*);
	
public:
	
	Libyuv() throw (AVException*);
	~Libyuv();
	Libyuv(int,int,AVRational,AVRational,PixelFormat) throw (AVException*);

	
	void allocFrameData(void) throw (AVException*);
	void setExtensions(int);
	int setOutputFilename(std::string) throw (AVException*);
	void setOutputFd(int); 
	void setWidth(int);
	void setHeight(int); 
	void setInterlacing(int);
	void setInterlaceAndOrder(int, int);
	void setChromaSampling (int );
	void setChromaSamplingFromAV(PixelFormat) throw (AVException*);
	void setFrameRate(y4m_ratio_t);
	void setFrameRateAVRational(AVRational);
	void setSampleAspect(y4m_ratio_t );
	void setSampleAspectAVRational(AVRational);
	int getHeight(void);
	int getWidth(void);
	int getChromaHeight(void);
	int getChromaWidth(void); 
	void setYUVFrameDataWithAVFrame(AVFrame *);
	int writeHeader(void) throw (AVException*);
	int write(void) throw (AVException*);
	void deallocFrameData(void);

};