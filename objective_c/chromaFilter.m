
#import "chromaFilter.h"

@implementation chromaFilter

- (id)initWithAVObject:(AVObject *)s
{
	
	[self init];

	source = s;
	[self setFrameRate:[s getFrameRate]];
	[self setSampleAspect:[s getSampleAspect]];
	[self setHeight:[s getHeight]];
	[self setWidth:[s getWidth]];
	[self setInterlaced:[s getIsInterlaced]];
	[self setInterlaceTopFieldFirst:[s getInterlaceTopFieldFirst]];
	
	return self;

}

- (id)initWithAVObject:(AVObject *)s toAVChroma:(int)convertMode
{

	if ([self initWithAVObject:s] != nil) 
	{
	
		[self setChromaSampling:convertMode];
	 
		[self allocFrame];
	 
		imgConvertCtx = sws_getContext([self getWidth], [self getHeight], [s getChromaSampling], 
									   [self getWidth], [self getHeight], convertMode, SWS_BICUBIC, NULL, NULL, NULL); 


		return self;
	}
	return nil;
}

- (id)initWithAVObject:(AVObject *)s toY4MChroma:(int)convertMode
{

	// need a chroma conversion function
	//	[self initWithAVObject:s toAVChroma: setChromaSamplingFromY4M:convertMode];
	
	// there should be a better way than this code duplication
	[self initWithAVObject:s];
	
	[self setChromaSamplingFromY4M:convertMode];
	
	[self allocFrame];
	
	imgConvertCtx = sws_getContext([self getWidth], [self getHeight], [s getChromaSampling], 
								   [self getWidth], [self getHeight], [self getChromaSampling], SWS_BICUBIC, NULL, NULL, NULL); 
	
	
	return self;
	
	
}

-(int)decodeNextFrame
{

	AVFrame *pFrameSource;
	
	[source decodeNextFrame];
	
	pFrameSource = [source getAVFrame];
	return sws_scale(imgConvertCtx,  (const uint8_t * const *)((AVPicture *)pFrameSource)->data,
					 pFrameSource->linesize, 0, [self getHeight],pFrame->data, pFrame->linesize);
	
}

- (void)dumpFormat
{
	NSLog(@"chroma conversion filter: %@\n",[self getChromaSamplingName]);
	[source dumpFormat];
}


@end