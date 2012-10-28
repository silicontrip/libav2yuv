
#import "chromaFilter.h"

@implementation chromaFilter

- (id)initWithAVObject:(AVObject *)s toChroma:(int)convertMode
{

	[self init];
	
	source = s;
	[self setFrameRate:[s getFrameRate]];
	[self setSampleAspect:[s getSampleAspect]];
	[self setHeight:[s getHeight]];
	[self setWidth:[s getWidth]];
	[self setChromaSampling:convertMode];
	[self setInterlaced:[s getIsInterlaced]];
	[self setInterlaceTopFieldFirst:[s getInterlaceTopFieldFirst]];
	 
	[self allocFrame];
	 
	imgConvertCtx = sws_getContext([self getWidth], [self getHeight], [s getChromaSampling], 
									 [self getWidth], [self getHeight], convertMode, SWS_BICUBIC, NULL, NULL, NULL); 


	return self;
}

-(int)decodeNextFrame
{

	AVFrame *pFrameSource;
	
	[source decodeNextFrame];
	
	pFrameSource = [source getAVFrame];
	//chromaFilter.m:38: warning: passing argument 2 of ‘sws_scale’ from incompatible pointer type
	// what am I doing wrong here? everywhere I look it's AVFrame->data 
	return sws_scale(imgConvertCtx,  (const uint8_t * const *)((AVPicture *)pFrameSource)->data, pFrameSource->linesize, 0, [self getHeight],pFrame->data, pFrame->linesize);
	
}

- (void)dumpFormat
{
	NSLog(@"chroma conversion filter: %@\n",[self getChromaSamplingName]);
	[source dumpFormat];
}


@end