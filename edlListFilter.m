#import "edlListFilter.h"

@implementation edlListFilter


- (id)init
{
	[super init];
	
	entries=[NSMutableArray arrayWithCapacity:1] ;
	
	return self;
}

- (id)initWithFile:(NSString *)filename
{

	[self init];
	
	NSString *fileContents = [NSString stringWithContentsOfFile:filename encoding:NSUTF8StringEncoding error:nil];
	NSArray *lines = [fileContents componentsSeparatedByString:@"\n"];
		
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
			
			libav *entry = [[libav alloc] initVideoWithFile:fileName];
			
			[entry setInTimecode:tcIn];
			[entry setOutTimecode:tcOut];
			
			
			[entry retain];
			//		NSLog(@"Time: %d - %d",[entry getIn],[entry getOut]);
			[entries addObject:entry];
		}
	}
	
	return self;
	
}

- (int)decodeNextFrame
{

	int bytes;
	
	while ((bytes=[[self currentAV] decodeNextFrame]) < 0) {
	
		[entries removeObjectAtIndex:0];
		if ([entries count] == 0)
			return -1;
	}
	return bytes;
}

- (void) dumpFormat
{
	NSLog(@"EDL List AVObject");
	for (AVObject *o in entries)
		[o dumpFormat];
}

- (AVObject *)currentAV { return[entries objectAtIndex:0];}

- (AVRational)getFrameRate { return [[self currentAV] getFrameRate]; }
- (int)getFrameRateNum { return [[self currentAV] getFrameRateNum]; }
- (int)getFrameRateDen { return [[self currentAV] getFrameRateDen]; }
- (AVRational)getSampleAspect { return [[self currentAV] getSampleAspect]; }
- (int)getSampleAspectNum { return [[self currentAV] getSampleAspectNum]; }
- (int)getSampleAspectDen { return [[self currentAV] getSampleAspectDen]; }
- (int)getChromaSampling { return [[self currentAV] getChromaSampling]; }
- (bool)getIsInterlaced { return [[self currentAV] getIsInterlaced]; }
- (bool)getInterlaceTopFieldFirst { return [[self currentAV] getInterlaceTopFieldFirst]; }
- (int)getHeight{ return [[self currentAV] getHeight]; };
- (int)getWidth{ return [[self currentAV] getWidth]; };
- (int)getChromaHeight{ return [[self currentAV] getChromaHeight]; };
- (int)getChromaWidth{ return [[self currentAV] getChromaWidth]; };
- (AVFrame *)getAVFrame{ return [[self currentAV] getAVFrame]; }


@end