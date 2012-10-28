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
	
	int frameWidth;
	int frameHeight;
	
	AVRational frameRate;
	AVRational sampleAspect;
	AVFrame *pFrame;
	uint8_t *pictureBuffer;

	int frameChromaSampling;
	bool isInterlaced;
	bool interlaceTopFieldFirst;
	
	}

- (id)init;
- (id)initWithChroma:(int)ch height:(int)h width:(int)w;
- (void) allocFrame;
- (void)dumpFormat;
- (AVRational)getFrameRate;
- (int)getFrameRateNum;
- (int)getFrameRateDen;
- (AVRational)getSampleAspect;
- (int)getSampleAspectNum;
- (int)getSampleAspectDen;
- (int)getChromaSampling;
- (NSString *)getChromaSamplingName;
- (bool)getIsInterlaced;
- (bool)getInterlaceTopFieldFirst;
- (int)getHeight;
- (int)getWidth;
- (int)getChromaHeight;
- (int)getChromaWidth;
- (int)getFrameCounter;
- (int)getIn;
- (int)getOut;

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
- (AVFrame *)getAVFrame;
- (AVFrame *)decodeAndGetNextAVFrame;
- (void)dealloc;
@end