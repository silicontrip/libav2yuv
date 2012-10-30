
#import <Foundation/NSArray.h>
#import "libav.h"
#import "AVObject.h"

@interface edlListFilter : AVObject {
	NSMutableArray *entries;
	int frameCounter;
}

- (id)init;
- (id)initWithFile:(NSString *)filename streamType:(int)st;
+ (BOOL)hasAudio:(NSString *)mode;
+ (BOOL)hasVideo:(NSString *)mode;

- (AVObject *)currentAV;

@end