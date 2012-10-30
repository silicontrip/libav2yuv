#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/Foundation.h>

#import "libav.h"
#import "libyuv.h"
#import "libav2yuvArguments.h"
#import "AVObject.h"
#import "chromaFilter.h"
#import "edlListFilter.h"

NSAutoreleasePool  *pool;

int main(int argc, char *argv[])
{
	
	NSMutableArray *edlList;
	pool = [[NSAutoreleasePool alloc] init];
	edlList = [NSMutableArray arrayWithCapacity:1];
	
	libav2yuvArguments *options = [[libav2yuvArguments alloc]  initWithNSProcessInfoArguments:[[NSProcessInfo processInfo] arguments]];
	
	
	for (NSString *argument in [options getArguments]) {
		AVObject *lav = nil;
		if ([argument hasSuffix:@".edl"]) {
			lav = [[edlListFilter alloc] initWithFile:argument];
		} else  {	
			lav = [[libav alloc] initVideoWithFile:argument];
		}
		if (lav != nil) 
		{
			if ([options getConvert]) 
			{
				chromaFilter *chromaConverter = [[chromaFilter alloc] initWithAVObject:lav toY4MChroma:[options getChroma]];
				if (chromaConverter != nil) {
					[edlList addObject:chromaConverter];
				}
			} else {
				[edlList addObject:lav];
			}
			
		}	
	}
	
	if ([edlList count] > 0 ) 
	{
		AVObject *lav = [edlList objectAtIndex:0];
		
		[lav dumpFormat];
		
		NSLog(@"%dx%d by %d:%d at %d:%d",[lav getWidth],[lav getHeight],[lav getSampleAspectNum],[lav getSampleAspectDen],[lav getFrameRateNum],[lav getFrameRateDen]);
		
		libyuv *yuv;
		
		// NSLog(@"libyuv alloc");
		yuv = [[libyuv alloc] 
			   initWithWidth:[lav getWidth] 
			   Height:[lav getHeight] 
			   SampleAspectAVRational:[lav getSampleAspect]
			   FrameRateAVRational:[lav getFrameRate]
			   ChromaFromAV:[lav getChromaSampling]];
		
		if ([options getExtensions])
		{
			[yuv setExtensions:1];
		}
		if ([options hasAspect]) {
			[yuv setSampleAspect:[options getAspect]];
		}
		if ([options hasFrameRate]) {
			[yuv setFrameRate:[options getFrameRate]];
		}
		if ([options hasChroma]){
			[yuv setChromaSampling:[options getChroma]];
		}		
		if ([options hasOutFile]){
			[yuv setOutputFilename:[options getOutFile]];
		}
		// need to decode the first frame to get the interlace type
		NSLog(@"lav decodeNextFrame");
		
		[lav decodeNextFrame];
		NSLog(@"yuv setYUVFrameDataWithAVFrame");
		
		[yuv setYUVFrameDataWithAVFrame:[lav getAVFrame]];
		NSLog(@"yuv setInterlaceAndOrder");
		
		
		
		[yuv setInterlaceAndOrder:[lav getIsInterlaced] topFieldFirst:[lav getInterlaceTopFieldFirst]];
		//interlace flag is not available until the first frame is decoded.
		//need to get the interlace flags before this. So we can use a generator.
		// NSLog(@"yuv writeHeader");
		
		[yuv writeHeader];
		[yuv write];
		// [lav release];
		
		for (AVObject *video in edlList) {
			if (video != nil) {
				
				
				while ([video decodeNextFrame] >=0)
				{
					[yuv setYUVFrameDataWithAVFrame:[video getAVFrame]];
					[yuv write];
					
				}
				
				[video release];
			} else {
				NSLog(@"Couldn't initialise video %s",argv[1]);
			}
			
		}
		[yuv release];
	}
	[pool release];
	
	
}
