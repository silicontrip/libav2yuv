
#import "chromaFilter.h"

@implementation chromaFilter

- (id)initwithAVObject:(AVObject *)s toChroma:(int)convertMode
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

	
}

-(int)decodeNextFrame
{

	AVFrame *pFrameSource;
	
	[source decodeNextFrame];
	
	pFrameSource = [source getAVFrame];
	sws_scale(imgConvertCtx,  pFrameSource->data, pFrameSource->linesize, 0, [self getHeight],pFrame->data, pFrame->linesize);

	
}

- (void)dumpFormat
{
	fprintf(stderr,"chroma conversion filter: ");
	[source dumpFormat];
}


@end