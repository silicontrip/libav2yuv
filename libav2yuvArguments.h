#import <Cocoa/Cocoa.h>
#import <Foundation/NSUserDefaults.h>

#include <yuv4mpeg.h>


@interface libav2yuvArguments : NSObject {

	BOOL extensions;
	BOOL audio;
	BOOL convert;

	int interlace;
	y4m_ratio_t frameRate;
	NSString *format;
	y4m_ratio_t aspect;
	int chroma;
	NSString *outFile;
	NSString *range;
	NSMutableArray *arguments;

	
	int stream;
	
}

- (id)initWithNSProcessInfoArguments:(NSArray *)a;
- (id)initWithNSUserDefaults:(NSUserDefaults *)args;

- (void)usage;
- (void)chromaModes;


- (NSArray *)getArguments;
- (int)getInterlace;
- (int)getChroma;
- (y4m_ratio_t)getFrameRate;
- (y4m_ratio_t)getAspect;
- (NSString *)getFormat;
- (NSString *)getOutFile;
- (BOOL)getExtensions;
- (BOOL)getAudio;
- (BOOL)getConvert;

- (BOOL)hasInterlace;
- (BOOL)hasChroma;
- (BOOL)hasFrameRate;
- (BOOL)hasAspect;
- (BOOL)hasOutFile;
- (BOOL)hasFormat;
- (BOOL)hasRange;
- (BOOL)hasStream;


@end