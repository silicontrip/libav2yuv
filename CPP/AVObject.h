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

	
	void dumpFormat(void);


	AVRational getFrameRate(void);
	int getFrameRateNum(void);
	int getFrameRateDen(void);
	AVRational getSampleAspect(void);
	int getSampleAspectNum(void);
	int getSampleAspectDen(void);
	const char * getChromaSamplingName(void);
	PixelFormat getChromaSampling(void);
	bool getIsInterlaced(void);
	bool getInterlaceTopFieldFirst(void);
	int getHeight(void);
	int getWidth(void);
	int getChromaHeight(void);
	int getChromaWidth(void);

	int getFrameCounter(void);
	int getIn(void);
	int getOut(void);
	int compareRange(int);
	int compareSamplesRange(int);
	int64_t getSamplesIn(void);
	int64_t getSamplesOut(void);
	int getSampleSize(void);
	int getSampleChannels(void);
	AVSampleFormat getSampleFormat(void);
	int getSamplesPerSecond(void);
	double getSamplesPerFrame(void);
	void setColour(uint8_t, uint8_t, uint8_t);

	void setIn(int);
	void setOut(int);
	void setSampleChannels(int);
	void setSampleFormat(AVSampleFormat);

	void setSamplesPerSecond(int);

	int TCtoFrames(std::string);
	void setInTimecode(std::string);
	void setOutTimecode(std::string);

	void setFrameRate(AVRational);
	void setFrameRateNum(int);
	void setFrameRateDen(int);

	void setSampleAspect(AVRational);
	void setSampleAspectNum(int);
	void setSampleAspectDen(int);

	void setChromaSampling(PixelFormat);
	void setChromaSamplingFromY4M(int);

	void setInterlaced(bool);
	void setInterlaceTopFieldFirst(bool);

	void setHeight(int);
	void setWidth(int);
	
	int decodeNextAudio(void);
	int decodeNextFrame(void);
	AVFrame * getAVFrame(void);
	AVFrame * decodeAndGetNextAVFrame(void);


};