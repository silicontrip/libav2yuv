extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <iostream>
#include <string>

class LibavWaveWriter {

protected:
	int samplesPerSecond;
	int channels;
	AVSampleFormat sampleFormat;
	
	AVFormatContext *oc;
	AVOutputFormat *libavfmt;
	AVStream *audio_st;
	
	
public:
	LibavWaveWriter();
	// LibavWaveWriter(AVSampleFormat,int,int);
	~LibavWaveWriter();
	int setOutputFilename(std::string);
	int writeFrameData(AVFrame *);
	void setSamplesPerSecond(int);
	void setSampleChannels(int);
	void setSampleFormat(AVSampleFormat);
	int writeHeader(void);
	
};