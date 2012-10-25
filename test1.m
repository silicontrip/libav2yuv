
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/Foundation.h>
#import "libav.h"
#import "libyuv.h"
#import "AVObject.h"


NSAutoreleasePool  *pool;

int main(int argc, char *argv[])
{
	
	libav *lav;
	
	pool = [[NSAutoreleasePool alloc] init];
	
	
	lav = [[libav alloc] initVideoWithFile:argv[1]];
	[lav setIn:30]; [lav setOut:59];
	
	if (lav != nil) {
		
		//[lav setIn:25];
		//[lav setOut:50];
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
			
				// need to decode the first frame to get the interlace type
				[lav decodeNextFrame];
				[yuv setInterlaceAndOrder:[lav getIsInterlaced] topFieldFirst:[lav getInterlaceTopFieldFirst]];
				
				//interlace flag is not available until the first frame is decoded.
				//need to get the interlace flags before this. So we can use a generator.
				[yuv writeHeader];
				
			AVObject *black;
			
			black = [[AVObject alloc] initWithChroma:[lav getChromaSampling] height:[lav getHeight] width:[lav getWidth]];
			
			if (black != nil) {
				[black setIn:0]; [black setOut:29];
				
				while ([black decodeNextFrame] >=0)
				{
					[yuv setYUVFrameDataWithAVFrame:[black getAVFrame]];
					[yuv write];
					
				}
				[black release];
			}
				[yuv setYUVFrameDataWithAVFrame:[lav getAVFrame]];
				[yuv write];
				
				while (	[lav decodeNextFrame] >= 0)
				{ 		
					[yuv setYUVFrameDataWithAVFrame:[lav getAVFrame]];
					[yuv write];
				}
				
			
			black = [[AVObject alloc] initWithChroma:[lav getChromaSampling] height:[lav getHeight] width:[lav getWidth]];
			
			if (black != nil) {
				[black setIn:0]; [black setOut:29];

				while ([black decodeNextFrame] >=0)
				{
					[yuv setYUVFrameDataWithAVFrame:[black getAVFrame]];
					[yuv write];
					
				}
				[black release];
			}
			[yuv release];
		}
		[lav release];
	} else {
		NSLog(@"Couldn't initialise video %s",argv[1]);
	}
	[pool release];
	
	
}
