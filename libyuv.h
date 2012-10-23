
#import <Cocoa/Cocoa.h>


@interface libyuv : NSObject {

	/*
	int yuvHeight;
	int yuvWidth;
	int yuvChromaHeight;
	int yuvChromaWidth;
*/
	y4m_stream_info_t yuvStreamInfo;
	y4m_frame_info_t yuvFrameInfo;
	uint8_t *frameData[3];
	int fdOut;
	bool fileHeaderWritten;

}
- (id)init;
- (id)initWithWidth:(int)w Height:(int)h Interlace:(int)i SampleAspect:(y4m_ratio_t)sa FrameRate:(y4m_ratio_t)fr Chroma:(int)ch;

- (void)setOutputFilename:(char *)filename;
- (void)setOutputFd:(int)fd;

- (void)setWidth:(int)w;
- (void)setHeight:(int)w;
- (void)setInterlacing:(int)i;
//- (void)setSampleAspectNum:(int)num;
//- (void)setSampleAspectDen:(int)den;
- (void)setSampleAspect:(y4m_ratio_t)ratio;
//- (void)setFrameRateNum:(int)num;
//- (void)setFrameRateDen:(int)den;
- (void)setFrameRate:(y4m_ratio_t)ratio;
- (void)setChromaSampling:(int)ch;

- (int)writeHeader;
- (int)write;

- (uint8_t **)getYUVFramePointer;
- (void)setYUVFramePointer(uint8_t **)m;


@end