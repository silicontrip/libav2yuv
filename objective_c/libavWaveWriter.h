#import <Cocoa/Cocoa.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>


@interface libavWaveWriter : NSObject {

	int samplesPerSecond;
	int channels;
	int sampleFormat;
	
	AVFormatContext *oc;
	AVOutputFormat *libavfmt;
	AVStream *audio_st;
	
}

- (id)initWithFormat:(int)fmt channels:(int)ch samplesPerSecond:(int)sps;


- (id)init;	



- (int)setOutputFilename:(NSString *)filename; 
// - (void)setOutputFd:(int)fd;


- (int)writeFrameData:(AVFrame *)pFrame;

- (void)setSamplesPerSecond:(int)sps;
- (void)setSampleChannels:(int)ch;
- (void)setSampleFormat:(int)fmt;

- (int)writeHeader;
- (void)dealloc ;



@end