#import <Cocoa/Cocoa.h>
#include <libavutil/rational.h>
#include <libavutil/pixfmt.h>
#include <stdint.h>

@interface AVObject : NSObject {

	int frameCounter;
	int frameIn;
	int frameOut;
	
	int frameWidth;
	int frameHeight;
	
	AVRational frameRate;
	AVRational sampleAspect;

	int frameChromaSampling;
	bool isInterlaced;
	bool interlaceTopFieldFirst;
	
	}

- (id)init;
- (id)initWithChroma:(int)ch height:(int)h width:(int)w;
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

- (void)setIn:(int)fin;
- (void)setOut:(int)fout;
- (void)setFrameRate:(AVRational)rational;
- (void)setFrameRateNum:(int)num;
- (void)setFrameRateDen:(int)den;
- (void)setSampleAspect:(AVRational)rational;
- (void)setSampleAspectNum:(int)num;
- (void)setSampleAspectDen:(int)den;
- (void)setChromaSampling:(int)samp;
- (void)setInterlaced:(bool)i;
- (void)setInterlaceTopFieldFirst:(bool)tff;
- (void)setHeight:(int)hei;
- (void)setWidth:(int)wid;

// trying to think of the default call to get frame data
// and behaviour when outside the set in out points.
- (int) decodeNextFrameToYUV:(uint8_t **)m;

@end