
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
	
	[self setChromaSampling:ch];
	[self setHeight:h];
	[self setWidth:w];
	
	[self setIn:0];
	[self setOut:INT32_MAX];
	
	[self allocFrame];
	
	if (!pFrame)
		return nil;
	
	return self;
}

- (void) allocFrame 
{
	pFrame=avcodec_alloc_frame();
	
	if (pFrame)
	{
		int ch = [self getChromaSampling];
		int h = [self getHeight];
		int w = [self getWidth];
		
		int size = avpicture_get_size(ch, w, h);
		//const char *fmt = av_get_pix_fmt_name(ch);
		//NSLog(@"picture buffer  %dx%d %s size %d" ,w,h,fmt, size);
		pictureBuffer = av_malloc(size);
		if (pictureBuffer) {
			avpicture_fill((AVPicture *)pFrame, pictureBuffer, ch, w, h);
		} else {
			av_free(pFrame);
			pFrame = nil;
		}
	}
}

- (void)dumpFormat
{
	fprintf(stderr,"Black generator AV Object: %dx%d",[self getWidth],[self getHeight]);	
}

- (AVRational)getFrameRate
{
	return frameRate;
}

- (int)getFrameRateNum {
	return frameRate.num;
}
- (int)getFrameRateDen {
	return frameRate.den;
}

- (AVRational)getSampleAspect
{
	return sampleAspect;
}

- (int)getSampleAspectNum {
	return sampleAspect.num;
}
- (int)getSampleAspectDen {
	return sampleAspect.den;
}

- (NSString *)getChromaSamplingName
{
	return [[NSString alloc] initWithUTF8String:av_get_pix_fmt_name([self getChromaSampling])];
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

- (int)TCtoFrames:(NSString*)timecode
{
	//determine if drop frame
	NSRange dropFrameRange = [timecode rangeOfString:@";"];
	BOOL dropFrame = TRUE;
	
	int frn = [self getFrameRateNum];
	int frd =  [self getFrameRateDen];
	
	
	if (!dropFrameRange.location) 
	{
		if ( 1.0 * frn / frd == 30000.0 / 1001.0) {
			frn = 30;
			frd = 1;
		}
		dropFrame = FALSE;
	}
	
	NSString *newTimecode = [timecode stringByReplacingOccurrencesOfString:@";" withString:@":"];
	
//	NSLog(@"new tc %@",newTimecode);
	
	NSArray *digits = [newTimecode componentsSeparatedByString:@":"];
	
//	NSLog(@"digits %@",digits);

	
	int frames = [[digits lastObject] intValue];
//	NSLog(@"Frames %d",frames);
	int seconds;
	
	if ([digits count] >1) {
		
		seconds = 0;
		int i;
		for (i=0 ; i<[digits count]-1; i++)
		{
			seconds = (seconds * 60) + 	 [[digits objectAtIndex:i] intValue];
//			NSLog(@"Seconds: %d",seconds);
		}
	}
	
	// need to account for drop frame.
	
	int frame = frames + seconds * frn / frd;
	
//	NSLog(@"out: %d",frame);
	
	return frame;
}

- (void) setInTimecode:(NSString *)sin
{
	[self setIn:[self TCtoFrames:sin]];
}

- (void)setOutTimecode:(NSString *)sout
{
	[self setOut:[self TCtoFrames:sout]];
}

- (void)setFrameRate:(AVRational)rational {
//	NSLog(@"setFrameRate: %d/%d",rational.num,rational.den);
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

- (void)setChromaSamplingFromY4M:(int)y4mChroma
{

	switch (y4mChroma) {
		case Y4M_CHROMA_420MPEG2: [self setChromaSampling:PIX_FMT_YUV420P]; break;
		case Y4M_CHROMA_422: [self setChromaSampling:PIX_FMT_YUV422P]; break;
		case Y4M_CHROMA_444 : [self setChromaSampling:PIX_FMT_YUV444P]; break;
		case Y4M_CHROMA_411: [self setChromaSampling:PIX_FMT_YUV411P]; break;
		case Y4M_CHROMA_420JPEG: [self setChromaSampling:PIX_FMT_YUVJ420P]; break;
			
		default:
			NSLog(@"AV: Unsupported Chroma");

			break;
	}
	
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
	//pFrame->height = hei;
}

- (void)setWidth:(int)wid {
	frameWidth = wid;
	//	pFrame->width = wid;
}

- (int)decodeNextFrame
{
	if (frameCounter < frameIn)
		frameCounter = frameIn;
	
	if (frameCounter <= frameOut) {
		
		memset(pFrame->data[0],16,[self getHeight]*pFrame->linesize[0]);
		memset(pFrame->data[1],128,[self getChromaHeight]*pFrame->linesize[1]);
		memset(pFrame->data[2],128,[self getChromaHeight]*pFrame->linesize[2]);
		frameCounter++;
		
		return [self getHeight]*pFrame->linesize[0] +
		[self getHeight]*pFrame->linesize[1] +
		[self getHeight]*pFrame->linesize[2];
	}
	return -1;
	
}

- (int)decodeNextFrameToYUV:(uint8_t **)m
{
	
	
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

- (AVFrame *)getAVFrame{ return pFrame; }
- (AVFrame *)decodeAndGetNextAVFrame{ [self decodeNextFrame]; [self getAVFrame]; }
- (void) dealloc
{
	if (pFrame)
		av_free(pFrame);
	if (pictureBuffer)
		av_free(pictureBuffer);
	[super dealloc];
}

@end