


@implementation libyuv 

- (id)init 
{
	self=[super init];
	y4m_init_stream_info (&yuvStreamInfo);
	fileHeaderWritten = FALSE;
	frameData=NULL;
	
	[self setOutputFd: 1]; // STDOUT
	
	return self;
}

- (id)initWithWidth:(int)w Height:(int)h Interlace:(int)i SampleAspect:(y4m_ratio_t)sa FrameRate:(y4m_ratio_t)fr Chroma:(int)ch
{
	[self init];
	
	[self setWidth:w];
	[self setHeight:h];
	[self setInterlacing:i];
	[self setSampleAspect:sa];
	[self setFrameRate:fr];
	[self setChromaSampling:ch];
	
	return self;
}

- (void)setOutputFilename:(char *)filename
{
	return -1;
}

- (void)setOutputFd:(int)fd 
{
	fdOut = fd;
}

- (void)setWidth:(int)w { y4m_si_set_width(yuvStreamInfo, w); }
- (void)setHeight:(int)w { y4m_si_set_height(yuvStreamInfo, h); }
- (void)setInterlacing:(int)i { y4m_si_set_interlace(yuvStreamInfo,i); }
- (void)setChromaSampling:(int)ch { y4m_si_set_chroma(yuvStreamInfo,ch); }
- (void)setFrameRate:(y4m_ratio_t)fr { y4m_si_set_framerate(yuvStreamInfo, fr); }
- (void)setSampleAspect:(y4m_ratio_t)sa { y4m_si_set_sampleaspect(yuvStreamInfo, sa); }

- (int)writeHeader 
{
	
	int write_error_code = y4m_write_stream_header(fdOut, yuvStreamInfo);
	if (write_error_code == Y4M_OK)
		headerWritten = TRUE;
	return write_error_code;
}

- (int)write
{
	return -1;
}

@end