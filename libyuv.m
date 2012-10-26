
#import "libyuv.h"

@implementation libyuv 

- (id)init 
{
	
	// NSLog(@"> libyuv init");
	self=[super init];
	y4m_init_stream_info (&yuvStreamInfo);
	y4m_init_frame_info(&yuvFrameInfo);
	
	fileHeaderWritten = FALSE;
	frameData = (uint8_t **)malloc(sizeof (uint8_t *) * 3);
	frameData[0]=nil;
	frameData[1]=nil;
	frameData[2]=nil;
	
	[self setOutputFd: 1]; // STDOUT
	//NSLog(@"< libyuv init");
	
	return self;
}

- (id)initWithWidth:(int)w Height:(int)h Chroma:(int)ch
{
	[self init];
	
	[self setWidth:w];
	[self setHeight:h];
	[self setChromaSampling:ch];
	[self allocFrameData];
	
	return self;
	
}

- (id)initWithWidth:(int)w Height:(int)h ChromaFromAV:(int)pix_fmt
{
	[self init];
	
	[self setWidth:w];
	[self setHeight:h];
	[self setChromaSamplingFromAV:pix_fmt];
	[self allocFrameData];
	
	return self;
	
}	

- (id)initWithWidth:(int)w Height:(int)h  SampleAspect:(y4m_ratio_t)sa FrameRate:(y4m_ratio_t)fr Chroma:(int)ch
{
	[self initWithWidth:w Height:h Chroma:ch];
	
	[self setSampleAspect:sa];
	[self setFrameRate:fr];
	[self allocFrameData];

	return self;
}

- (id)initWithWidth:(int)w Height:(int)h SampleAspectAVRational:(AVRational)sa FrameRateAVRational:(AVRational)fr Chroma:(int)ch
{
	[self initWithWidth:w Height:h Chroma:ch];
	
	[self setSampleAspectAVRational:sa];
	[self setFrameRateAVRational:fr];

	[self allocFrameData];

	return self;
	
}

- (id)initWithWidth:(int)w Height:(int)h SampleAspectAVRational:(AVRational)sa FrameRateAVRational:(AVRational)fr ChromaFromAV:(int)pix_fmt
{
	[self initWithWidth:w Height:h ChromaFromAV:pix_fmt];
	
	[self setSampleAspectAVRational:sa];
	[self setFrameRateAVRational:fr];
	
	[self allocFrameData];
	
	return self;
	
}


- (void)allocFrameData
{
	
	int fs,cfs;
	
	fs = y4m_si_get_plane_length(&yuvStreamInfo,0);
	cfs = y4m_si_get_plane_length(&yuvStreamInfo,1);
	
	
	frameData[0] = (uint8_t *)malloc( fs );
	frameData[1] = (uint8_t *)malloc( cfs);
	frameData[2] = (uint8_t *)malloc( cfs);
	
}


- (int)setOutputFilename:(char *)filename
{
	
	fdOut = open(filename,O_WRONLY|O_CREAT);
	return fdOut;
	
}

- (void)setOutputFd:(int)fd 
{
	fdOut = fd;
}

- (void)setWidth:(int)w {  y4m_si_set_width(&yuvStreamInfo, w);  }
- (void)setHeight:(int)h {  y4m_si_set_height(&yuvStreamInfo, h);  }
- (void)setInterlacing:(int)i { y4m_si_set_interlace(&yuvStreamInfo,i); }
- (void)setInterlaceAndOrder:(int)i topFieldFirst:(int)tff
{
	
	if (i) {
		if (tff) {
			[self setInterlacing:Y4M_ILACE_TOP_FIRST];
		} else {
			[self setInterlacing:Y4M_ILACE_BOTTOM_FIRST];
		}
	} else {
		[self setInterlacing:Y4M_ILACE_NONE];
	}
	
	
}

- (void)setChromaSampling:(int)ch { y4m_si_set_chroma(&yuvStreamInfo,ch); }
- (void)setChromaSamplingFromAV:(int)pix_fmt
{

	switch (pix_fmt) 
	{
		case PIX_FMT_YUV420P: [self setChromaSampling:Y4M_CHROMA_420MPEG2]; break;
		case PIX_FMT_YUV422P: [self setChromaSampling:Y4M_CHROMA_422]; break;
		case PIX_FMT_YUV444P: [self setChromaSampling:Y4M_CHROMA_444]; break;
		case PIX_FMT_YUV411P: [self setChromaSampling:Y4M_CHROMA_411]; break;
		case PIX_FMT_YUVJ420P: [self setChromaSampling:Y4M_CHROMA_420JPEG]; break;
		default:
			NSLog(@"Unsupported Chroma");
			break;	
	}
}
- (void)setFrameRate:(y4m_ratio_t)fr { 
	
	if (fr.d==0 && fr.n==0) {
		fr.d = 1;
		fr.n=1;
	}
	
	y4m_si_set_framerate(&yuvStreamInfo, fr); 
}
- (void)setFrameRateAVRational:(AVRational)rational
{
	y4m_ratio_t fr;
	
	fr.d = rational.den;
	fr.n = rational.num;
	[self setFrameRate:fr];
}

- (void)setSampleAspect:(y4m_ratio_t)sa { 
	
	// check for invalid aspect ratio
	if (sa.d==0 && sa.n==0) {
		sa.d = 1;
		sa.n=1;
	}
	
	y4m_si_set_sampleaspect(&yuvStreamInfo, sa); 
}
- (void)setSampleAspectAVRational:(AVRational)rational
{
	y4m_ratio_t sa;
	
	sa.d = rational.den;
	sa.n = rational.num;
	[self setSampleAspect:sa];
}

- (int)getHeight { return y4m_si_get_plane_height(&yuvStreamInfo,0); }
- (int)getWidth { return y4m_si_get_plane_width(&yuvStreamInfo,0); }
- (int)getChromaHeight { return y4m_si_get_plane_height(&yuvStreamInfo,1); }
- (int)getChromaWidth { return y4m_si_get_plane_width(&yuvStreamInfo,1); }

- (void)setYUVFrameDataWithAVFrame:(AVFrame *)pFrame
{
	
	int y;
	int h = [self getHeight];
	int w = [self getWidth];
	int ch = [self getChromaHeight];
	int cw = [self getChromaWidth];
	for (y=0; y<h; y++) {
		//		mjpeg_debug ("copy %d bytes to: %x from: %x",w,dst[0]+y*w,(src->data[0])+y*src->linesize[0]);
		//	NSLog(@"memcpy0 %d %d %x %x",w,pFrame->linesize[0],m[0],pFrame->data[0]);
		
		memcpy(frameData[0]+y*w,(pFrame->data[0])+y*pFrame->linesize[0],w);
		if (y<ch) {
			
#ifdef DEBUG
			mjpeg_debug("copy %d bytes to: %x from: %x",cw,dst[1]+y*cw,(src->data[1])+y*src->linesize[1]);
#endif
			//	NSLog(@"memcpy1");
			
			memcpy(frameData[1]+y*cw,(pFrame->data[1])+y*pFrame->linesize[1],cw);
			//	NSLog(@"memcpy2");
			
			memcpy(frameData[2]+y*cw,(pFrame->data[2])+y*pFrame->linesize[2],cw);
		}
	}
	
	
}


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


- (void)deallocFrameData
{
	if (frameData != nil)
	{
		if (frameData[0] != nil)
			free(frameData[0]);
		if (frameData[1] != nil)
			free(frameData[1]);
		if (frameData[2] != nil)
			free(frameData[2]);
		
		free(frameData);
	}
}	

- (void)dealloc
{
	
	[self deallocFrameData];
	[super dealloc];
}

@end