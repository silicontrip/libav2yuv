#import "edlListFilter.h"

@implementation edlListFilter


- (id)init
{
	[super init];
	
	entries=[NSMutableArray arrayWithCapacity:1] ;
	frameCounter = 0;
	return self;
}

- (id)initWithFile:(NSString *)filename streamType:(int)st 
{
	
	if ([self init]!=nil) {
	
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
			
			
			libav *entry;
			if ((st == AVMEDIA_TYPE_VIDEO) && [self hasVideo:mode])
			{
				NSLog(@"Adding Video entry: %@",fileName);
				entry = [[libav alloc] initWithFile:fileName streamType:AVMEDIA_TYPE_VIDEO stream:-1];
			}
			
			if ((st == AVMEDIA_TYPE_AUDIO) && [self hasAudio:mode])
			{
				NSLog(@"Adding Audio entry: %@",fileName);

				entry = [[libav alloc] initWithFile:fileName streamType:AVMEDIA_TYPE_AUDIO stream:-1];
			}
			
			[entry setInTimecode:tcIn];
			[entry setOutTimecode:tcOut];
			
			[entry retain];
			NSLog(@"Time: %d - %d",[entry getIn],[entry getOut]);
			[entries addObject:entry];
		}
	}
	
	return self;
	}
	return nil;
}

- (BOOL)hasAudio:(NSString *)mode
{

	NSRange enableAudio = [mode rangeOfString:@"A"];
	NSRange enableaudio = [mode rangeOfString:@"a"];
	NSRange enableBoth = [mode rangeOfString:@"B"];
	NSRange enableboth = [mode rangeOfString:@"B"];
	
	
	if (enableAudio.location || enableaudio.location || enableBoth.location || enableboth.location) 
		return TRUE;
	
	return FALSE;
}

- (BOOL)hasVideo:(NSString *)mode
{

	NSRange enableVideo = [mode rangeOfString:@"V"];
	NSRange enablevideo = [mode rangeOfString:@"v"];
	NSRange enableBoth = [mode rangeOfString:@"B"];
	NSRange enableboth = [mode rangeOfString:@"B"];
	
	if (enableVideo.location || enablevideo.location || enableBoth.location || enableboth.location) 
		return TRUE;
	
	return FALSE;
	
}

- (int)decodeNextFrame
{
	
	if ([self compareRange:frameCounter]>0)
		return -1;
	
	int bytes;
	do {
		while ((bytes=[[self currentAV] decodeNextFrame]) < 0) {
			
			[entries removeObjectAtIndex:0];
			if ([entries count] == 0)
				return -1;
		}
		frameCounter++;
	} while ([self compareRange:frameCounter]<0);
	return bytes;
}

- (int)decodeNextAudio
{
	
	// NSLog(@"edlListFilter decodeNextAudio sampleCounter %d entriesCount %d",sampleCounter,[entries count]);
	
	if ([self compareSamplesRange:sampleCounter] > 0)
		return -1;
	
	int samples;
	do {
		while ((samples=[[self currentAV] decodeNextAudio]) < 0) {
			
			[entries removeObjectAtIndex:0];
			if ([entries count] == 0)
				return -1;
		}
		sampleCounter +=samples; 
	
	} while ([self compareSamplesRange:sampleCounter] < 0);
	return samples;
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
- (int)getSampleSize{ return [[self currentAV] getSampleSize]; }
- (int)getSampleChannels{ return [[self currentAV] getSampleChannels]; }
- (int)getSampleFormat{ return [[self currentAV] getSampleFormat]; }
- (int)getSamplesPerSecond{ return [[self currentAV] getSamplesPerSecond]; }
- (double)getSamplesPerFrame{ return [[self currentAV] getSamplesPerFrame]; }



@end