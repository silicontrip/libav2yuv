#import "AVObject.h"
#include "libswscale/swscale.h"

@interface chromaFilter : AVObject {

	AVObject *source;
	struct SwsContext *imgConvertCtx;
	
}

- (id)initwithAVObject:(AVObject *)source toChroma:(int)convertMode;

@end