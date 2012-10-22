
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/Foundation.h>
#import "libav.h"


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

	
	
		[lav release];
	} else {
		NSLog(@"Couldn't initialise video %s",argv[1]);
	}
	[pool release];

	
}
