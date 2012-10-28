#import "AVObject.h"
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>


@interface chromaFilter : AVObject {

	AVObject *source;
	struct SwsContext *imgConvertCtx;
	
}

- (id)initWithAVObject:(AVObject *)source toChroma:(int)convertMode;

@end