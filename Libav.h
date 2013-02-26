extern "C" {
#include <stdint.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <string>

#include "AVException.h"
#include "AVObject.h"

#define BUFFER_SIZE 16384


class Libav: public AVObject {

protected:
	AVFormatContext *pFormatCtx;
	AVInputFormat *avif;
	AVCodecContext *pCodecCtx;
	AVCodec *pCodec;
	AVPacket packet;
	
	std::string lavFileName;
	
	int avStream;
	int streamType;
	
	int16_t		*aBuffer;
	
	int openInputFile(std::string);
	int findStreamInfo(void);
	int findStream(int,int);
	int openAVCodec(void);

	
public:
	Libav(std::string,int,int) throw (AVException*);
	~Libav();

	
	void dumpFormat(void);
	std::string getFilename(void);
	int decodeNextAudio(void) throw (AVException*);
	int decodeNextFrame(void) throw (AVException*);
};