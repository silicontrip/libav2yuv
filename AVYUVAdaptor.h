
extern "C" {
#include <yuv4mpeg.h>
#include <mpegconsts.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
}

#include <string>
#include <iostream>
#include "AVException.h"


class AVYUVAdaptor {

public:
    // should be Y4M not YUV 
#if LIBAVFORMAT_VERSION_MAJOR  < 57	
    static int AVPixelFormatToYUVChroma(PixelFormat) throw (AVException*);
#else
    static int AVPixelFormatToYUVChroma(AVPixelFormat) throw (AVException*);
#endif
    static const y4m_ratio_t AVRationalToY4mRatio (AVRational);
    static void copyAVFrameToY4MFrame (uint8_t ** , AVFrame *);
    static int interlaceAndOrderToY4M (int i, int tff);

};
