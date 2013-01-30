extern "C" {
#include <yuv4mpeg.h>
#include <mpegconsts.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <fcntl.h>
}

#include <string>
#include <iostream>

class Libyuv {

protected:
	y4m_stream_info_t yuvStreamInfo;
	y4m_frame_info_t yuvFrameInfo;
	uint8_t **frameData;
	int fdOut;
	bool fileHeaderWritten;

public:
	
	Libyuv();
	~Libyuv();

	void allocFrameData(void);
	void setExtensions(int);
	int setOutputFilename(std::string);
	void setOutputFd(int); 
	void setWidth(int);
	void setHeight(int); 
	void setInterlacing(int);
	void setInterlaceAndOrder(int, int);
	void setChromaSampling (int );
	void setChromaSamplingFromAV(PixelFormat);
	void setFrameRate(y4m_ratio_t);
	void setFrameRateAVRational(AVRational);
	void setSampleAspect(y4m_ratio_t );
	void setSampleAspectAVRational(AVRational);
	int getHeight(void);
	int getWidth(void);
	int getChromaHeight(void);
	int getChromaWidth(void); 
	void setYUVFrameDataWithAVFrame(AVFrame *);
	int writeHeader(void);
	int write(void);
	void deallocFrameData(void);

};