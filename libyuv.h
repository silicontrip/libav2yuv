
#import <Cocoa/Cocoa.h>
#include <yuv4mpeg.h>
#include <mpegconsts.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <fcntl.h>


@interface libyuv : NSObject {

	/*
	int yuvHeight;
	int yuvWidth;
	int yuvChromaHeight;
	int yuvChromaWidth;
*/
	y4m_stream_info_t yuvStreamInfo;
	y4m_frame_info_t yuvFrameInfo;
	uint8_t **frameData;
	int fdOut;
	bool fileHeaderWritten;

}
- (id)init;
- (id)initWithWidth:(int)w Height:(int)h SampleAspect:(y4m_ratio_t)sa FrameRate:(y4m_ratio_t)fr Chroma:(int)ch;
- (id)initWithWidth:(int)w Height:(int)h SampleAspectAVRational:(AVRational)sa FrameRateAVRational:(AVRational)fr Chroma:(int)ch;
- (id)initWithWidth:(int)w Height:(int)h SampleAspectAVRational:(AVRational)sa FrameRateAVRational:(AVRational)fr ChromaFromAV:(int)pix_fmt;
- (id)initWithWidth:(int)w Height:(int)h Chroma:(int)ch;
- (id)initWithWidth:(int)w Height:(int)h ChromaFromAV:(int)pix_fmt;

- (void)allocFrameData;

- (void)setExtensions:(int)ext ;
- (int)setOutputFilename:(NSString *)filename; // returns -1 on failure
- (void)setOutputFd:(int)fd;

- (void)setWidth:(int)w;
- (void)setHeight:(int)h;
- (void)setInterlacing:(int)i;
- (void)setInterlaceAndOrder:(int)i topFieldFirst:(int)tff;
- (void)setSampleAspect:(y4m_ratio_t)sa;
- (void)setSampleAspectAVRational:(AVRational)rational;
- (void)setFrameRate:(y4m_ratio_t)fr;
- (void)setFrameRateAVRational:(AVRational)rational;
- (void)setChromaSampling:(int)ch;
- (void)setChromaSamplingFromAV:(int)pix_fmt;
- (int)writeHeader;
- (int)write;
- (int)getHeight;
- (int)getWidth;
- (int)getChromaHeight;
- (int)getChromaWidth;
- (void)setYUVFrameDataWithAVFrame:(AVFrame *)pFrame;
- (void)deallocFrameData;
- (void)dealloc;


@end