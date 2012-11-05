
#import "libav2yuvArguments.h"

@implementation libav2yuvArguments

- (id)init
{

	[super init];
	
	extensions = 0;
	interlace = -1;
	frameRate.d = 0;
	format = nil;
	aspect.d = 0;
	chroma = -1;
	outFile = nil;
	range = nil;
	stream = -1;
	
	
}

- (id)initWithNSProcessInfoArguments:(NSArray *)a
{

	[self init];
	
	arguments = [NSMutableArray arrayWithCapacity:[a count]];
	NSMutableArray *args = [NSMutableArray arrayWithCapacity:[a count]];
	
	[args addObjectsFromArray:a];
	[args removeObjectAtIndex:0];

	while ([args count] > 0) {
		NSString *argument = [args objectAtIndex:0];
		
		if ([argument compare:@"-extensions"]==0 || [argument compare:@"-E"]==0) {
			extensions = 1;
		} else if ([argument compare:@"-audio"]==0 || [argument compare:@"-w"]==0) {
			audio = 1;
		} else if ([argument compare:@"-convert"]==0 || [argument compare:@"-c"]==0) {
			convert = 1;
		} else if ([argument compare:@"-interlace"]==0 || [argument compare:@"-I"]==0) {
			[args removeObjectAtIndex:0];
			NSString *intString =  [args objectAtIndex:0];
			if([intString hasPrefix:@"p"] || [intString hasPrefix:@"P"])
			{
				interlace = Y4M_ILACE_NONE;
			} else if ([intString hasPrefix:@"t"] || [intString hasPrefix:@"T"]) {
				interlace = Y4M_ILACE_TOP_FIRST;
			} else if([intString hasPrefix:@"b"] || [intString hasPrefix:@"B"]) {
				interlace = Y4M_ILACE_BOTTOM_FIRST;
			} else {
				return nil;
			}
			
		} else if ([argument compare:@"-frameRate"]==0 || [argument compare:@"-F"]==0) {
			[args removeObjectAtIndex:0];
			NSString *optarg =  [args objectAtIndex:0];
			if( Y4M_OK != y4m_parse_ratio(&frameRate, [optarg UTF8String]) ) {
				return nil;
			}
		} else if ([argument compare:@"-aspect"]==0 || [argument compare:@"-A"]==0) {
			[args removeObjectAtIndex:0];
			NSString *aspectString = [args objectAtIndex:0];
			if( Y4M_OK != y4m_parse_ratio(&aspect, [aspectString UTF8String]) ) {
				if([aspectString compare:@"PAL"] == 0) {
					y4m_parse_ratio(&aspect, [@"128:117" UTF8String]);
				} else if ([aspectString compare:@"PAL_WIDE"] == 0) {
					y4m_parse_ratio(&aspect, [@"512:351" UTF8String]);
				} else if ([aspectString compare:@"NTSC"] == 0) {
					y4m_parse_ratio(&aspect, [@"4320:4739" UTF8String]);
				} else if ([aspectString compare:@"NTSC_WIDE"] == 0) {
					y4m_parse_ratio(&aspect, [@"5760:4739" UTF8String]);
				} else {
					return nil;
				}
			}
		} else if ([argument compare:@"-chroma"]==0 || [argument compare:@"-S"]==0) {
			[args removeObjectAtIndex:0];
			NSString *optarg = [args objectAtIndex:0];
			chroma = y4m_chroma_parse_keyword([optarg UTF8String]);
			if (chroma == Y4M_UNKNOWN) {
				NSLog(@"Unknown Chroma Mode: %@",optarg);
				[self chromaModes];
				return nil;
			}
		} else if ([argument compare:@"-format"]==0 || [argument compare:@"-f"]==0) {
			[args removeObjectAtIndex:0];
			format =  [NSString stringWithString:[args objectAtIndex:0]];
		} else if ([argument compare:@"-out"]==0 || [argument compare:@"-o"]==0) {
			//NSLog(@"process outfile");
			[args removeObjectAtIndex:0];
			outFile =  [NSString stringWithString:[args objectAtIndex:0]];
		} else if ([argument compare:@"-range"]==0 || [argument compare:@"-r"]==0) {
			[args removeObjectAtIndex:0];
			range =  [NSString stringWithString:[args objectAtIndex:0]];
		} else if ([argument compare:@"-stream"]==0 || [argument compare:@"-s"]==0) {
			[args removeObjectAtIndex:0];
			NSString *optarg = [args objectAtIndex:0];
			stream = [optarg intValue];
		} else if ([argument hasPrefix:@"-"]) {
			NSLog(@"Unknown argument: %@",argument);
			[self usage];
			return nil;
		} else {
			[arguments addObject:argument];
		}
		[args removeObjectAtIndex:0];
		
	}
	return self;
}


- (void)usage
{
	fprintf (stderr,
			 "usage: libav2yuv [-stream<stream>] [-interlace p|b|t] [-frameRate <rate>] [-aspect <aspect>] [-chroma <chroma>] [-out <outputfile>] <filename>\n"
			 "converts any media file recognised by libav to yuv4mpeg stream\n"
			 "\n"
			 "\t -audio Write a PCM file not a video file\n"
			 "\t -interlace <pbt> Force interlace mode overides parameters read from media file\n"
			 "\t -frameRate <n:d> Force framerate. This is not\n"
			 "\t -format <fmt> Force format type (if incorrectly detected)\n"
			 "\t -aspect (<n:d>|PAL|PAL_WIDE|NTSC|NTSC_WIDE) Force aspect ratio\n"
			 "\t -chroma <chroma> Force chroma subsampling mode. Only use this if you know what you are doing, or using -convert\n"
			 "\t   if the mode in the stream is unsupported will upsample to YUV444\n"
			 "\t -convert Force conversion to chroma mode (requires a chroma mode)\n"
			 "\t -stream select stream other than stream 0\n"
			 "\t -out <outputfile> write to file rather than stdout\n"
			 "\t -range [[[HH:]MM:]SS:]FF-[[[HH:]MM:]SS:]FF playout only these frames\n"
			 "\t -extension enable y4m extensions (may be required if source file is not a common format)\n"
			 "\t -help print this help\n"
			 );
	
}

-(void)chromaModes
{
	fprintf(stderr,
			"VALID Chroma Values:\n"
			"\t420jpeg   (default)\n"
			"\t420mpeg2\n"
			"\t420paldv\n"
			"\t411\n"
			"\t422\n"
			"\t444       - non-subsampled Y'CbCr\n"
			"\t444alpha  - Y'CbCr with alpha channel (with Y' black/white point)\n"
			);
}

- (NSArray *)getArguments{ return arguments; }
- (int)getInterlace { return interlace; }
- (int)getChroma { return chroma; }
- (y4m_ratio_t)getFrameRate { return frameRate; }
- (y4m_ratio_t)getAspect { return aspect; }
- (NSString *)getFormat { return format; }
- (NSString *)getOutFile { return outFile; }
- (BOOL)getExtensions { return extensions; }
- (BOOL)getAudio { return audio; }
- (BOOL)getConvert { return convert; }

- (BOOL)hasInterlace { return interlace != -1; }
- (BOOL)hasChroma { return chroma != -1; }
- (BOOL)hasFrameRate { return frameRate.d != 0; }
- (BOOL)hasAspect { return aspect.d != 0; }
- (BOOL)hasOutFile { return outFile != nil; }
- (BOOL)hasFormat { return format != nil; }
- (BOOL)hasRange { return range != nil; }
- (BOOL)hasStream { return stream != -1; }
@end
