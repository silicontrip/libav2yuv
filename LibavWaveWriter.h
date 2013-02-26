extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "AVException.h"
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
	LibavWaveWriter() throw (AVException*);
	// LibavWaveWriter(AVSampleFormat,int,int);
	~LibavWaveWriter();
	int setOutputFilename(std::string) throw (AVException*);
	int writeFrameData(AVFrame *) throw (AVException*);
	void setSamplesPerSecond(int);
	void setSampleChannels(int);
	void setSampleFormat(AVSampleFormat);
	int writeHeader(void) throw (AVException*);
	
};