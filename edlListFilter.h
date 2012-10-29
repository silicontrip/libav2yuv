
#import <Foundation/NSArray.h>
#import "libav.h"
#import "AVObject.h"

@interface edlListFilter : AVObject {
	NSMutableArray *entries;
}

- (id)init;
- (id)initWithFile:(NSString *)filename;
- (AVObject *)currentAV;

@end