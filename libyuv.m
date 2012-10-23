
#import "libyuv.h"

@implementation libyuv 

- (id)init 
{
	
	// NSLog(@"> libyuv init");
	self=[super init];
	y4m_init_stream_info (&yuvStreamInfo);
	y4m_init_frame_info(&yuvFrameInfo);

	fileHeaderWritten = FALSE;
	//frameData[0]=NULL;
	//frameData[1]=NULL;
	//frameData[2]=NULL;

	[self setOutputFd: 1]; // STDOUT
	//NSLog(@"< libyuv init");

	return self;
}

- (id)initWithWidth:(int)w Height:(int)h  SampleAspect:(y4m_ratio_t)sa FrameRate:(y4m_ratio_t)fr Chroma:(int)ch
{
	[self init];
	
	[self setWidth:w];
	[self setHeight:h];
	[self setSampleAspect:sa];
	[self setFrameRate:fr];
	[self setChromaSampling:ch];
	
	return self;
}

- (void)allocFrameData
{

	int fs,cfs;
	
	fs = y4m_si_get_plane_length(&yuvStreamInfo,0);
	cfs = y4m_si_get_plane_length(&yuvStreamInfo,1);
	
	frameData = (uint8_t **)malloc(sizeof (uint8_t *) * 3);
	
	frameData[0] = (uint8_t *)malloc( fs );
	frameData[1] = (uint8_t *)malloc( cfs);
	frameData[2] = (uint8_t *)malloc( cfs);
	
}


- (int)setOutputFilename:(char *)filename
{
	return -1;
}

- (void)setOutputFd:(int)fd 
{
	fdOut = fd;
}

- (void)setWidth:(int)w { y4m_si_set_width(&yuvStreamInfo, w); }
- (void)setHeight:(int)h { y4m_si_set_height(&yuvStreamInfo, h); }
- (void)setInterlacing:(int)i { y4m_si_set_interlace(&yuvStreamInfo,i); }
- (void)setChromaSampling:(int)ch { y4m_si_set_chroma(&yuvStreamInfo,ch); }
- (void)setFrameRate:(y4m_ratio_t)fr { y4m_si_set_framerate(&yuvStreamInfo, fr); }
- (void)setSampleAspect:(y4m_ratio_t)sa { y4m_si_set_sampleaspect(&yuvStreamInfo, sa); }

- (int)writeHeader 
{
	
	int write_error_code = y4m_write_stream_header(fdOut, &yuvStreamInfo);
	if (write_error_code == Y4M_OK)
		fileHeaderWritten = TRUE;
	return write_error_code;
}

- (int)write
{
	int write_error_code;

	write_error_code = y4m_write_frame(fdOut, &yuvStreamInfo, &yuvFrameInfo, frameData); 

	return write_error_code;
}

- (uint8_t **)getYUVFramePointer
{
	return frameData;	
}

- (void)setYUVFramePointer:(uint8_t **)m
{
	frameData = m;
}

@end