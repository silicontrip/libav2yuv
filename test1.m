
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
		
		[lav dumpFormat];
		/*	
		 NSLog(@"frame rate: %d:%d", [lav getFrameRateNum], [lav getFrameRateDen]);
		 NSLog(@"aspect rate: %d:%d", [lav getSampleAspectNum], [lav getSampleAspectDen]);
		 NSLog(@"chroma: %d", [lav getChromaSampling]);
		 NSLog(@"dim: %dx%d", [lav getWidth],[lav getHeight]);
		 NSLog(@"chroma dim: %dx%d", [lav getChromaWidth],[lav getChromaHeight]);
		 */
		libyuv *yuv;
		
		yuv = [[libyuv alloc] initWithWidth:[lav getWidth] 
									 Height:[lav getHeight] 
					 SampleAspectAVRational:[lav getSampleAspect]
						FrameRateAVRational:[lav getFrameRate]
									 Chroma:[lav getChromaSampling]];
		if (yuv != nil) {
			[yuv allocFrameData];
			
			[lav decodeNextFrameToYUV:[yuv getYUVFramePointer]];
			
			//interlace flag is not available until the first frame is decoded.
			[yuv setInterlaceAndOrder:[lav getIsInterlaced] topFieldFirst:[lav getInterlaceTopFieldFirst]];
			[yuv writeHeader];
			[yuv write];
			while (	[lav decodeNextFrameToYUV:[yuv getYUVFramePointer]] >= 0)
				[yuv write];
			
			[yuv release];
		}
		[lav release];
	} else {
		NSLog(@"Couldn't initialise video %s",argv[1]);
	}
	[pool release];
	
	
}
