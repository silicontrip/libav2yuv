#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/Foundation.h>
#import "libav.h"
#import "libyuv.h"
#import "libav2yuvArguments.h"
#import "AVObject.h"
#import "chromaFilter.h"

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
			
			// I really need an NSString interface for this
			libav *entry = [[libav alloc] initVideoWithFile:fileName];
			
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
	
	NSUserDefaults *args = [NSUserDefaults standardUserDefaults];
	
	//NSLog(@"libav2yuvArguments alloc");
	
	libav2yuvArguments *options = [[libav2yuvArguments alloc] initWithArguments:args];
	[args release];
	
	
	NSArray *argArray = [[NSProcessInfo processInfo] arguments];
//	NSLog(@"This program is named %@.", [argArray objectAtIndex:0]);
//	NSLog(@"There are %d arguments.", [argArray count] - 1);
	
	
	int c;
	for (c =1; c < [argArray count]; c++) {
	
		NSString *argument = [argArray objectAtIndex:c];
	
		NSLog(@"argument: %@",argument);
		if ([argument hasPrefix:@"-"]) {
			NSLog(@"Unknown option: %@",argument);
			[options usage];
		} else if ([argument hasSuffix:@".edl"]) {
			[edlList addObjectsFromArray:parseEdl(argument)];
		} else if ([argument length] >0) {	
			if ([options getConvert]) 
			{
				chromaFilter *chromaConverter = [[chromaFilter alloc] initwithAVObject:[[libav alloc] initVideoWithFile:argument] toChroma:[options getChroma]];
				if (chromaConverter != nil) 
				[edlList addObject:chromaConverter];
				
			} else {
				libav *lav = [[libav alloc] initVideoWithFile:argument];
				if (lav != nil) 
				[edlList addObject:lav];
			}
			
		} else {
			[options usage];
		}
	}
	
	if ([edlList count] > 0 ) 
	{
		AVObject *lav = [edlList objectAtIndex:0];
		
		[lav dumpFormat];
		
		NSLog(@"%dx%d by %d:%d at %d:%d",[lav getWidth],[lav getHeight],[lav getSampleAspectNum],[lav getSampleAspectDen],[lav getFrameRateNum],[lav getFrameRateDen]);
		
		libyuv *yuv;
		
		// NSLog(@"libyuv alloc");
		yuv = [[libyuv alloc] initWithWidth:[lav getWidth] 
			   Height:[lav getHeight] 
			   SampleAspectAVRational:[lav getSampleAspect]
			   FrameRateAVRational:[lav getFrameRate]
			   ChromaFromAV:[lav getChromaSampling]];
		
		if ([options hasAspect]) 
		[yuv setSampleAspect:[options getAspect]];
		
		if ([options hasFrameRate]) 
		[yuv setFrameRate:[options getFrameRate]];
		
		if ([options hasChroma])
		[yuv setChromaSampling:[options getChroma]];
		
		if ([options hasOutFile])
		[yuv setOutputFilename:[options getOutFile]];
		
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
