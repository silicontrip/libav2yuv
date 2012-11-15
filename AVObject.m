
#import "AVObject.h"

@implementation AVObject 

- (id)init
{
	if([super init] != nil )
	{
		
		[self setIn:-1];
		[self setOut:-1];
		
		[self setColourY:16 U:128 V:128]; // set default colour to black
		
		return self;
	}
	return nil;
}

- (id)initWithSilence:(int)samples channels:(int)ch sampleFormat:(int)sf samplesPerSecond:(int)sps
{
	
	pFrame=avcodec_alloc_frame();
	avcodec_get_frame_defaults(pFrame);
	
	[self setSampleChannels:ch];
	[self setSampleFormat:sf];
	[self setSamplesPerSecond:sps];
	
	bufferSize = samples;
	
	pictureBuffer = av_malloc(samples *
							  av_get_bytes_per_sample(sf) *
							  ch); 
	
	if (!pictureBuffer) 
	{
		NSLog(@"Unable to allocate buffer");
		[self release];
		return nil;
	}
	avcodec_fill_audio_frame(pFrame, ch, sf,
                             pictureBuffer,
                             samples *
                             av_get_bytes_per_sample(sf) *
                             ch, 1);
	
	memset(	pictureBuffer,0,samples *
		   av_get_bytes_per_sample(sf) *
		   ch);
	
}

- (id)initWithChroma:(int)ch height:(int)h width:(int)w
{
	[self init];
	
	[self setChromaSampling:ch];
	[self setHeight:h];
	[self setWidth:w];
	
	[self setIn:-1];
	[self setOut:-1];
	
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

- (int) compareRange:(int)fr
{
	
	if (frameIn == -1 && frameOut == -1) 
		return 0;
	
	if (fr < frameIn && frameIn != -1) 
		return -1;
	if (fr > frameOut && frameOut != -1)
		return 1;
	// there should be no other conditions.
	
	//	if (((fr >= frameIn) || (frameIn == -1)) && ((fr <= frameOut) || (frameOut == -1)))
	return 0;
	
}

- (int) compareSamplesRange:(int)fr
{
	
	if (frameIn == -1 && frameOut == -1) 
		return 0;
	
	double spf = [self getSamplesPerFrame];
	
	int samplesIn= frameIn * spf;
	int samplesOut= frameOut * spf;

	//NSLog(@"Compare samples Range: %d (%d - %d) ",fr,samplesIn,samplesOut);
	
	
	if (fr <  samplesIn && frameIn != -1) 
		return -1;
	if (fr >= samplesOut && frameOut != -1)
		return 1;
	
	return 0;
	
}

- (int64_t)getSamplesIn {
	return frameIn * [self getSamplesPerFrame];
}

- (int64_t)getSamplesOut {
	return frameOut * [self getSamplesPerFrame];
}

- (int)getSampleSize { return av_get_bytes_per_sample(sampleFormat); }
- (int)getSampleChannels { return sampleChannels; }
- (int)getSampleFormat { return sampleFormat; }
- (int)getSamplesPerSecond {return samplesPerSecond; }

- (double)getSamplesPerFrame { return 1.0 * samplesPerSecond * [self getFrameRateDen] / [self getFrameRateNum]; }


- (void)setColourY:(uint8_t)y U:(uint8_t)u V:(uint8_t)v
{
	colour_y = y;
	colour_u = u;
	colour_v = v;
	
	// create picture buffer here...
}


- (void)setIn:(int)fin {
	// validate
	
	// if (fin >= 0)
		frameIn = fin;
}

- (void)setOut:(int)fout {
	// validate
	
	//if (fout >= 0)
		frameOut = fout;
}

- (void)setSampleChannels:(int)ch
{
	sampleChannels = ch;
}

- (void)setSampleFormat:(int)fmt
{
	sampleFormat = fmt;
}


- (void) setSamplesPerSecond:(int)sps
{
	samplesPerSecond = sps;
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
	NSArray *digits = [newTimecode componentsSeparatedByString:@":"];
	
	int frames = [[digits lastObject] intValue];
	int seconds;
	
	if ([digits count] >1) {
		
		seconds = 0;
		int i;
		for (i=0 ; i<[digits count]-1; i++)
		{
			seconds = (seconds * 60) + 	 [[digits objectAtIndex:i] intValue];
		}
	}
	
	// need to account for drop frame.
	
	int frame = frames + seconds * frn / frd;
	
	
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
}

- (void)setWidth:(int)wid {
	frameWidth = wid;
}

- (int)decodeNextAudio
{
	// how to handle the in and out.
	
	if (sampleCounter > [self getSamplesOut])
		return -1;
	
	// do
	int samples = bufferSize / [self getSampleSize] * [self getSampleChannels];
	if ((sampleCounter < [self getSamplesIn]) && (sampleCounter + samples >[self getSamplesOut]))
	{
		// send partial frame
		// copy sampleBuffer + [self getSamplesIn] - sampleCounter to (sampleCounter + samples) - [self getSamplesOut]
		// bytes = [self getSamplesOut] - [self getSamplesIn]
	}
	else if ((sampleCounter < [self getSamplesIn]) && (sampleCounter + samples >=[self getSamplesIn]))
	{
		// send partial frame
	} 
	else if ((sampleCounter <=[self getSamplesOut]) && (sampleCounter + samples >[self getSamplesOut]))
	{
		// send partial frame
	}
	else 
	{
		// send entire frame.
	}
	// sampleCount += samples. 
}

- (int)decodeNextFrame
{
	if ([self compareRange:frameCounter] < 0)
		frameCounter = frameIn;
	
	// I only need to do this once.
	if ([self compareRange:frameCounter] == 0) {
		
		memset(pFrame->data[0],colour_y,[self getHeight]*pFrame->linesize[0]);
		memset(pFrame->data[1],colour_u,[self getChromaHeight]*pFrame->linesize[1]);
		memset(pFrame->data[2],colour_v,[self getChromaHeight]*pFrame->linesize[2]);
		frameCounter++;
		
		return [self getHeight]*pFrame->linesize[0] +
		[self getHeight]*pFrame->linesize[1] +
		[self getHeight]*pFrame->linesize[2];
	}
	return -1;
	
}

- (AVFrame *)getAVFrame{ return pFrame; }
- (AVFrame *)decodeAndGetNextAVFrame{ [self decodeNextFrame]; [self getAVFrame]; }
- (void) freeAVFrame { av_free(pFrame); }
- (void) dealloc
{
	if (pFrame)
		av_free(pFrame);
	if (pictureBuffer)
		av_free(pictureBuffer);
#if LIBAVFORMAT_VERSION_MAJOR  < 54
	if (aBuffer)
		av_free(aBuffer);
#endif
	[super dealloc];
}

@end