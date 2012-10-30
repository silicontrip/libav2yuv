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

void processVideo (libav2yuvArguments *options, NSArray *edlList)
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
	// NSLog(@"lav decodeNextFrame");
	
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
			NSLog(@"Couldn't initialise video");
		}
		
	}
	[yuv release];
	
	
}

int main(int argc, char *argv[])
{
	
	NSMutableArray *edlList;
	pool = [[NSAutoreleasePool alloc] init];
	edlList = [NSMutableArray arrayWithCapacity:1];
	int streamMode;
	
	libav2yuvArguments *options = [[libav2yuvArguments alloc]  initWithNSProcessInfoArguments:[[NSProcessInfo processInfo] arguments]];
	
	if ([options getAudio] ) {
		streamMode = AVMEDIA_TYPE_AUDIO;
	} else {
		streamMode = AVMEDIA_TYPE_VIDEO;
	}
	
	for (NSString *argument in [options getArguments]) {
		AVObject *lav = nil;
		if ([argument hasSuffix:@".edl"]) {
			lav = [[edlListFilter alloc] initWithFile:argument streamType:streamMode];
		} else  {	
			lav = [[libav alloc] initWithFile:argument streamType:streamMode stream:-1];
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
		
		
		if (streamMode == AVMEDIA_TYPE_VIDEO) {
			processVideo(options,edlList);
		} else {
			
			NSFileHandle *nfh;
			if ([options hasOutFile]){
				nfh = [NSFileHandle fileHandleForWritingAtPath:[options getOutFile]];
			} else {
				nfh = [NSFileHandle fileHandleWithStandardOutput];
			}
			
			// decode audio
			
			AVObject *lav = [edlList objectAtIndex:0];
			
			[lav dumpFormat];
			
			// open out file
			
			for (AVObject *audio in edlList) {
				if (audio != nil) {
					while ([audio decodeNextAudio] >=0)
					{
					
						AVFrame *pFrame = [audio getAVFrame];
					
						NSData *data = [NSData dataWithBytes:pFrame->data[0] length:pFrame->nb_samples];
					
						[nfh writeData:data];
					}
					[audio release];
				} else {
					NSLog(@"Couldn't initialise video %s",argv[1]);
				}
				
			}
			// close file
			[nfh closeFile];
			
		}
	}
	[pool release];
	
	
}
