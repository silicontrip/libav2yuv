
#import "AVObject.h"

@implementation AVObject 

- (id)init
{
	self = [super init];

	[self setIn:0];
	[self setOut:INT32_MAX];
	
	return self;
	
}

- (id)initWithChroma:(int)ch height:(int)h width:(int)w
{
	self = [super init];

//	[self setFrameRate:fr];
//	[self setSampleAspect:sa];
	[self setChromaSampling:ch];
	[self setHeight:h];
	[self setWidth:w];
	
	[self setIn:0];
	[self setOut:INT32_MAX];


	return self;
}

- (int)getFrameRateNum {
	return frameRate.num;
}
- (int)getFrameRateDen {
	return frameRate.den;
}

- (int)getSampleAspectNum {
	return sampleAspect.num;
}
- (int)getSampleAspectDen {
	return sampleAspect.den;
}
- (int)getChromaSampling {
	return frameChromaSampling;
}

- (bool)getIsInterlaced {
	return isInterlaced;
}

- (bool)getInterlaceTopFieldFirst {
	return interlaceTopFieldFirst;
}

- (int)getHeight {
	return frameHeight;
}
- (int)getWidth {
	return frameWidth;
}

-(int)getChromaHeight {
	//dependent on chroma
	switch ([self getChromaSampling]) {
		case PIX_FMT_YUV420P:
		case PIX_FMT_YUVJ420P:
		case PIX_FMT_YUVA420P:
		case PIX_FMT_YUV440P:
		case PIX_FMT_YUVJ440P:
		case PIX_FMT_YUV420P16LE:
		case PIX_FMT_YUV420P16BE:
		case PIX_FMT_NV12:
		case PIX_FMT_NV21:
			return frameHeight>>1;
			break;
		case PIX_FMT_YUV410P:
			return frameHeight >>2;
			break;
		default:
			return frameHeight;
			break;
	}
}


-(int)getChromaWidth {
	//dependent on chroma
	switch ([self getChromaSampling]) {
		case PIX_FMT_YUV420P:
		case PIX_FMT_YUYV422:
		case PIX_FMT_YUV422P:
		case PIX_FMT_YUVJ420P:
		case PIX_FMT_YUVJ422P:
		case PIX_FMT_UYVY422:
		case PIX_FMT_YUVA420P:
		case PIX_FMT_YUV420P16LE:
		case PIX_FMT_YUV420P16BE:
		case PIX_FMT_YUV422P16LE:
		case PIX_FMT_YUV422P16BE:
		case PIX_FMT_NV12:
		case PIX_FMT_NV21:
			return frameWidth>>1;
			break;
		case PIX_FMT_YUV410P:
		case PIX_FMT_YUV411P:
		case PIX_FMT_UYYVYY411:
			return frameWidth >>2;
			break;
		default:
			return frameHeight;
			break;
	}
}



- (int)getFrameCounter {
	return frameCounter;
}

- (int) getIn {
	return frameIn;
}

- (int) getOut {
	return frameOut;
}

- (void)setIn:(int)fin {
	// validate
	if (fin >= 0)
		frameIn = fin;
}

- (void)setOut:(int)fout {
	// validate
	if (fout >= 0)
		frameOut = fout;
}

- (void)setFrameRate:(AVRational)rational {
	if (rational.den>0 && rational.num>0)
	{
		frameRate.num = rational.num;
		frameRate.den = rational.den;
	}
}

- (void)setFrameRateNum:(int)num {
	if (num > 0)
		frameRate.num = num;
}

- (void)setFrameRateDen:(int)den {
	if (den > 0)
		frameRate.den = den;
}

- (void)setSampleAspect:(AVRational)rational {
	if (rational.den>0 && rational.num>0)
	{
		sampleAspect.num = rational.num;
		sampleAspect.den = rational.den;
	}
}


- (void)setSampleAspectNum:(int)num {
	if (num > 0)
		sampleAspect.num = num;
}

- (void)setSampleAspectDen:(int)den {
	if (den > 0)
		sampleAspect.den = den;
}

- (void)setChromaSampling:(int)samp {
	frameChromaSampling = samp;
}

- (void)setInterlaced:(bool)i
{
	isInterlaced = i;
}

- (void)setInterlaceTopFieldFirst:(bool)tff
{
	interlaceTopFieldFirst = tff;
}


- (void)setHeight:(int)hei {
	frameHeight = hei;
}

- (void)setWidth:(int)wid {
	frameWidth = wid;
}

- (int)decodeNextFrameToYUV:(uint8_t **)m
{

	if (frameCounter < frameIn)
		frameCounter = frameIn;
	
	if (frameCounter <= frameOut) {
		int frameLength = [self getHeight] * [self getWidth];
		int chromaLength = [self getChromaHeight] * [self getChromaWidth];
	
		memset(m[0],16,frameLength);
		memset(m[1],128,frameLength);
		memset(m[2],128,frameLength);
	
		frameCounter++;
		return frameLength + chromaLength<<1;
	}
	
	return -1;
	
}

@end