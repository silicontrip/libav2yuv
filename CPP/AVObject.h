extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include <libavutil/rational.h>
#include <libavutil/pixfmt.h>
#include <libavutil/pixdesc.h>
#include <stdint.h>
#include <yuv4mpeg.h>
}
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

class AVObject {

protected:
	int frameCounter;
	int frameIn;
	int frameOut;
	
	int samplesPerSecond;
	int sampleChannels;
	AVSampleFormat sampleFormat;
	int64_t sampleCounter;
	int bufferSize;
	int frameWidth;
	int frameHeight;
	
	AVRational frameRate;
	AVRational sampleAspect;
	AVFrame *pFrame;
	uint8_t *pictureBuffer;
	
	uint8_t colour_y;
	uint8_t colour_u;
	uint8_t colour_v;
	
#if LIBAVFORMAT_VERSION_MAJOR  < 54
	uint8_t *aBuffer;
#endif
	
	PixelFormat frameChromaSampling;
	bool isInterlaced;
	bool interlaceTopFieldFirst;
	
	void allocFrame(void);
	void freeAVFrame(void);

	
public:
	
	AVObject();
	AVObject(int, int, AVSampleFormat, int);
	AVObject(PixelFormat, int, int);
	
	~AVObject();

	virtual AVRational getFrameRate(void);
	virtual int getFrameRateNum(void);
	virtual int getFrameRateDen(void);
	virtual AVRational getSampleAspect(void);
	virtual int getSampleAspectNum(void);
	virtual int getSampleAspectDen(void);
	virtual const char * getChromaSamplingName(void);
	virtual PixelFormat getChromaSampling(void);
	virtual bool getIsInterlaced(void);
	virtual bool getInterlaceTopFieldFirst(void);
	virtual int getHeight(void);
	virtual int getWidth(void);
	virtual int getChromaHeight(void);
	virtual int getChromaWidth(void);

	virtual int getFrameCounter(void);
	virtual int getIn(void);
	virtual int getOut(void);
	virtual int compareRange(int);
	virtual int compareSamplesRange(int);
	virtual int64_t getSamplesIn(void);
	virtual int64_t getSamplesOut(void);
	virtual int getSampleSize(void);
	virtual int getSampleChannels(void);
	virtual AVSampleFormat getSampleFormat(void);
	virtual int getSamplesPerSecond(void);
	virtual double getSamplesPerFrame(void);
	void setColour(uint8_t, uint8_t, uint8_t);

	virtual void setIn(int);
	virtual void setOut(int);
	virtual void setSampleChannels(int);
	virtual void setSampleFormat(AVSampleFormat);

	virtual void setSamplesPerSecond(int);

	int TCtoFrames(std::string);
	void setInTimecode(std::string);
	void setOutTimecode(std::string);
	void setInOutTimecode(std::string tc);

	virtual void setFrameRate(AVRational);
	virtual void setFrameRateNum(int);
	virtual void setFrameRateDen(int);

	virtual void setSampleAspect(AVRational);
	virtual void setSampleAspectNum(int);
	virtual void setSampleAspectDen(int);

	virtual void setChromaSampling(PixelFormat);
	virtual void setChromaSamplingFromY4M(int);

	virtual void setInterlaced(bool);
	virtual void setInterlaceTopFieldFirst(bool);

	virtual void setHeight(int);
	virtual void setWidth(int);

	
	virtual	void dumpFormat(void);
	virtual	int decodeNextAudio(void);
	virtual	int decodeNextFrame(void);
	
	virtual AVFrame * getAVFrame(void);
	virtual AVFrame * decodeAndGetNextAVFrame(void);

	

};