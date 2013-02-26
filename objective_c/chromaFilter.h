#import "AVObject.h"
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>


@interface chromaFilter : AVObject {

	AVObject *source;
	struct SwsContext *imgConvertCtx;
	
}

- (id)initWithAVObject:(AVObject *)s;
- (id)initWithAVObject:(AVObject *)s toAVChroma:(int)convertMode;
- (id)initWithAVObject:(AVObject *)s toY4MChroma:(int)convertMode;


@end