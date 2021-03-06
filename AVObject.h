#pragma once
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#if LIBAVFORMAT_VERSION_MAJOR  < 57	
#else
#include <libavutil/imgutils.h>
#endif

#include <libavutil/rational.h>
#include <libavutil/pixfmt.h>
#include <libavutil/pixdesc.h>
#include <stdint.h>
#include <yuv4mpeg.h>
}
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <sstream>

#include "AVException.h"

struct timecodeStruct {
	int h;
	int m;
	int s;
	int f;
	bool df;
};

class AVObject {

private:
	int frameIn;
	int frameOut;
	
	// may have to change this again.
	// to handle NTSC drop frame timecode
//	AVRational secondsIn;
//	AVRational secondsOut;
	std::string timecodeIn;
	std::string timecodeOut;


	int samplesPerSecond;
	int sampleChannels;
	AVSampleFormat sampleFormat;
	int bufferSize;
	int frameWidth;
	int frameHeight;
	
	AVRational frameRate;
	AVRational sampleAspect;
	
	uint8_t colour_y;
	uint8_t colour_u;
	uint8_t colour_v;
	
#if LIBAVFORMAT_VERSION_MAJOR  < 54
	uint8_t *aBuffer;
#endif
#if LIBAVFORMAT_VERSION_MAJOR  < 57	
	PixelFormat frameChromaSampling;
#else
	AVPixelFormat frameChromaSampling;
#endif
	
	bool isInterlaced;
	bool interlaceTopFieldFirst;
	
protected:
	
	int64_t sampleCounter;
	int frameCounter;
	AVFrame *pFrame;
	uint8_t *pictureBuffer;

public:
	
	AVObject();
	AVObject(int, int, AVSampleFormat, int) throw (AVException*);
#if LIBAVFORMAT_VERSION_MAJOR  < 57	
	AVObject(PixelFormat, int, int) throw (AVException*);
#else
	AVObject(AVPixelFormat, int, int) throw (AVException*);
#endif
	
	virtual ~AVObject();

	std::string getSamplingAspectAsString(void);
	std::string getFrameRateAsString(void);

	
	virtual bool hasFrameRate(void);
	virtual AVRational getFrameRate(void);
	virtual int getFrameRateNum(void);
	virtual int getFrameRateDen(void);
	virtual bool hasSampleAspect(void);

	virtual AVRational getSampleAspect(void);
	virtual int getSampleAspectNum(void);
	virtual int getSampleAspectDen(void);
	virtual bool hasChromaSampling(void);
	virtual const std::string getChromaSamplingName(void);
#if LIBAVFORMAT_VERSION_MAJOR  < 57	
	virtual PixelFormat getChromaSampling(void);
#else
	virtual AVPixelFormat getChromaSampling(void);
#endif
	//virtual bool hasInterlace(void);
	virtual int getInterlace(void);
	virtual bool getIsInterlaced(void);
	virtual bool getInterlaceTopFieldFirst(void);
	virtual bool hasHeight(void);
	virtual int getHeight(void);
	virtual bool hasWidth(void);
	virtual int getWidth(void);
	virtual int getChromaHeight(void);
	virtual int getChromaWidth(void);

	virtual int getFrameCounter(void);
	virtual bool hasIn(void);
	virtual bool hasOut(void);

	virtual int getIn(void);
	virtual int getOut(void);
	virtual int compareRange(int);
	virtual int compareSamplesRange(int64_t);
	virtual int64_t getSamplesIn(void);
	virtual int64_t getSamplesOut(void);
	virtual int getSampleSize(void);
	virtual int getSampleChannels(void);
	virtual AVSampleFormat getSampleFormat(void);
    virtual void setRequestSampleFormat(AVSampleFormat sf);
	virtual int getSamplesPerSecond(void);
	virtual double getSamplesPerFrame(void);
	void setColour(uint8_t, uint8_t, uint8_t);

	virtual void setIn(int);
	virtual void setOut(int);
	// virtual void setIn(AVRational);
	// virtual void setOut(AVRational);

	virtual void setSampleChannels(int);
	virtual void setSampleFormat(AVSampleFormat);

	virtual void setSamplesPerSecond(int);
	virtual void open(void);

	// AVRational TCtoSecondsFrames(std::string) throw (AVException*);
	struct timecodeStruct TCtoStruct(std::string) throw (AVException*); 
	int TCtoFrames(std::string) throw (AVException*);
	int StructToFrames (struct timecodeStruct tc) throw (AVException *);
	std::string FramesToTC(int);
	std::string getInTimecode(void);
	std::string getOutTimecode(void);

	
	void setInTimecode(std::string) throw (AVException*);
	void setOutTimecode(std::string) throw (AVException*);
	void setInOutTimecode(std::string tc) throw (AVException*);

	virtual void setFrameRate(AVRational) throw (AVException*);
	virtual void setFrameRateNum(int) throw (AVException*);
	virtual void setFrameRateDen(int) throw (AVException*);

	virtual void setSampleAspect(AVRational) throw (AVException*);
	virtual void setSampleAspectNum(int) throw (AVException*);
	virtual void setSampleAspectDen(int) throw (AVException*);

#if LIBAVFORMAT_VERSION_MAJOR  < 57	
	virtual void setChromaSampling(PixelFormat);
#else
	virtual void setChromaSampling(AVPixelFormat);
#endif
	virtual void setChromaSamplingFromY4M(int) throw (AVException*);

	virtual void setInterlaced(bool);
	virtual void setInterlaceTopFieldFirst(bool);

	virtual void setHeight(int);
	virtual void setWidth(int);

	virtual	void dumpFormat(void);
	virtual	int decodeNextAudio(void);
	virtual	int decodeNextFrame(void);
	
	virtual AVFrame * getAVFrame(void);
	virtual AVFrame * decodeAndGetNextAVFrame(void);
	
	void allocFrame(void) throw (AVException*);
	void freeAVFrame(void);

};
