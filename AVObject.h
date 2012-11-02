#import <Cocoa/Cocoa.h>
#import <Foundation/NSString.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include <libavutil/rational.h>
#include <libavutil/pixfmt.h>
#include <libavutil/pixdesc.h>
#include <stdint.h>
#include <yuv4mpeg.h>


@interface AVObject : NSObject {

	int frameCounter;
	int frameIn;
	int frameOut;
	
	int samplesPerSecond;
	int sampleChannels;
	int sampleFormat;
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
	
	int frameChromaSampling;
	bool isInterlaced;
	bool interlaceTopFieldFirst;
	
	}

- (id)init;
- (id)initWithSilence:(int)samples channels:(int)ch sampleFormat:(int)sf samplesPerSecond:(int)sps;
- (id)initWithChroma:(int)ch height:(int)h width:(int)w;
- (void) allocFrame;
- (void)dumpFormat;
- (NSString *)getChromaSamplingName;
- (AVRational)getFrameRate;
- (int)getFrameRateNum;
- (int)getFrameRateDen;
- (AVRational)getSampleAspect;
- (int)getSampleAspectNum;
- (int)getSampleAspectDen;
- (int)getChromaSampling;
- (bool)getIsInterlaced;
- (bool)getInterlaceTopFieldFirst;
- (int)getHeight;
- (int)getWidth;
- (int)getChromaHeight;
- (int)getChromaWidth;
- (int)getFrameCounter;
- (int)getIn;
- (int)getOut;
- (int64_t)getSamplesIn;
- (int64_t)getSamplesOut;
- (int)getSampleSize;
- (int)getSampleChannels;

- (void)setColourY:(uint8_t)y U:(uint8_t)u V:(uint8_t)v;
- (void)setSamplesPerSecond:(int)sps;
- (void)setSampleChannels:(int)ch;
- (void)setSampleFormat:(int)fmt;


- (void)setIn:(int)fin;
- (void)setOut:(int)fout;
- (void)setInTimecode:(NSString *)sin;
- (void)setOutTimecode:(NSString *)sout;
- (void)setFrameRate:(AVRational)rational;
- (void)setFrameRateNum:(int)num;
- (void)setFrameRateDen:(int)den;
- (void)setSampleAspect:(AVRational)rational;
- (void)setSampleAspectNum:(int)num;
- (void)setSampleAspectDen:(int)den;
- (void)setChromaSampling:(int)samp;
- (void)setChromaSamplingFromY4M:(int)y4mChroma;

- (void)setInterlaced:(bool)i;
- (void)setInterlaceTopFieldFirst:(bool)tff;
- (void)setHeight:(int)hei;
- (void)setWidth:(int)wid;

// trying to think of the default call to get frame data
// and behaviour when outside the set in out points.
- (int)decodeNextFrame;
- (int)decodeNextAudio;
- (AVFrame *)getAVFrame;
- (AVFrame *)decodeAndGetNextAVFrame;
- (void)dealloc;
@end