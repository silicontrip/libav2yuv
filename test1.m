#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/Foundation.h>
#import "libav.h"
#import "libyuv.h"
#import "AVObject.h"


NSAutoreleasePool  *pool;

NSMutableArray *parseEdl (NSString *filename)
{
	
	NSString *fileContents = [NSString stringWithContentsOfFile:filename encoding:NSUTF8StringEncoding error:nil];
	NSArray *lines = [fileContents componentsSeparatedByString:@"\n"];
	
	NSMutableArray *edl = [NSMutableArray arrayWithCapacity:1];
	
	[edl retain];
	
	for (NSString* o in lines)
	{
		
		//	NSLog(@"line: %@",o);
		
		if (![o hasPrefix:@"#"] && [o length]>0) {
			
			NSString *fileName;
			NSString *mode;
			NSString *transition;
			NSString *tcIn;
			NSString *tcOut;
			
			NSScanner *aScanner = [NSScanner scannerWithString:o];
			
			[aScanner scanUpToString:@" " intoString:&fileName];
			[aScanner scanUpToString:@" " intoString:&mode];
			[aScanner scanUpToString:@" " intoString:&transition];
			[aScanner scanUpToString:@" " intoString:&tcIn];
			[aScanner scanUpToString:@" " intoString:&tcOut];


		//	NSArray *items = [o componentsSeparatedByString:@" "];
			//		NSLog(@"filename: (%@)",fileName);
			//		NSLog(@"in: %@",tcIn);
			//		NSLog(@"out: %@",tcOut);
			
			// I really need an NSString interface for this
			libav *entry = [[libav alloc] initVideoWithFile:[fileName UTF8String]];
			
			[entry setInTimecode:tcIn];
			[entry setOutTimecode:tcOut];
			
			
			[entry retain];
			//		NSLog(@"Time: %d - %d",[entry getIn],[entry getOut]);
			[edl addObject:entry];
		}
	}
	
	return edl;
	
}

int main(int argc, char *argv[])
{
	
	NSMutableArray *edlList;
	pool = [[NSAutoreleasePool alloc] init];
	edlList = [NSMutableArray arrayWithCapacity:1];
	
	
	int c;
	for (c =1; c < argc; c++ ) {
		NSString *file = [[NSString alloc] initWithUTF8String:argv[c]];
		
		if ([file hasSuffix:@".edl"]) {
			[edlList addObjectsFromArray:parseEdl(file)];
		} else {	
			libav *lav = [[libav alloc] initVideoWithFile:argv[c]];
			if (lav != nil) 
				[edlList addObject:lav];
			
		}
	}
	
	if ([edlList count] > 0 ) 
	{
		AVObject *lav = [edlList objectAtIndex:0];
		
		[lav dumpFormat];
		
		libyuv *yuv;
		
		// NSLog(@"libyuv alloc");
		yuv = [[libyuv alloc] initWithWidth:[lav getWidth] 
									 Height:[lav getHeight] 
					 SampleAspectAVRational:[lav getSampleAspect]
						FrameRateAVRational:[lav getFrameRate]
									 Chroma:[lav getChromaSampling]];
		
		// need to decode the first frame to get the interlace type
	//	NSLog(@"lav decodeNextFrame");
		
		[lav decodeNextFrame];
		// NSLog(@"yuv setYUVFrameDataWithAVFrame");
		
		[yuv setYUVFrameDataWithAVFrame:[lav getAVFrame]];
		// NSLog(@"yuv setInterlaceAndOrder");
		
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
