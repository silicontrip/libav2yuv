
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/Foundation.h>
#import "libav.h"
#import "libyuv.h"


NSAutoreleasePool  *pool;

int main(int argc, char *argv[])
{

	libav *lav;

	pool = [[NSAutoreleasePool alloc] init];

	
	lav = [[libav alloc] initVideoWithFile:argv[1]];
	
	if (lav != nil) {
	
		//[lav readFrame];
		//[lav decodeFrame];
		[lav dumpFormat];
	
		NSLog(@"frame rate: %d:%d", [lav getFrameRateNum], [lav getFrameRateDen]);
		NSLog(@"aspect rate: %d:%d", [lav getSampleAspectNum], [lav getSampleAspectDen]);
		NSLog(@"chroma: %d", [lav getChromaSampling]);
		NSLog(@"dim: %dx%d", [lav getWidth],[lav getHeight]);
		NSLog(@"chroma dim: %dx%d", [lav getChromaWidth],[lav getChromaHeight]);

		libyuv *yuv;
		
		y4m_ratio_t sa;
		y4m_ratio_t fr;
		
		int yuv_interlacing;
		
		//NSLog(@"sample aspect");
		
		sa.d = [lav getSampleAspectDen];
		sa.n = [lav getSampleAspectNum];
		
		//NSLog(@"frame rate");

		fr.d = [lav getFrameRateDen];
		fr.n = [lav getFrameRateNum];
		
		//NSLog(@"readFrame");

		[lav readFrame];
		
		//NSLog(@"decodeFrame");

		[lav decodeFrame];
		
		//NSLog(@"getIsInterlaced");

		if ([lav getIsInterlaced]) {
			if ([lav getInterlaceTopFieldFirst]) {
				yuv_interlacing = Y4M_ILACE_TOP_FIRST;
			} else {
				yuv_interlacing = Y4M_ILACE_BOTTOM_FIRST;
			}
		} else {
			yuv_interlacing = Y4M_ILACE_NONE;
		}
		
		//NSLog(@"libav alloc");

		yuv = [[libyuv alloc] initWithWidth:[lav getWidth] Height:[lav getHeight] Interlace:yuv_interlacing SampleAspect:sa FrameRate:fr  Chroma:[lav getChromaSampling]];
		//NSLog(@"writeHeader");

		[yuv writeHeader];
		//NSLog(@"release");

		[yuv release];
		[lav release];
	} else {
		NSLog(@"Couldn't initialise video %s",argv[1]);
	}
	[pool release];

	
}
