extern "C" {
#include <stdint.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <string>
#include <map>

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
	
    std::map<std::string, std::string> meta;
    
   // typedef pair<std::string, std::string> metaDict;
    
	std::string lavFileName;
	
	int isOpen;
	int avStream;
	int streamType;
	
	int16_t *aBuffer;
	
	int openInputFile(std::string);
	int findStreamInfo(void);
	int findStream(int,int);
	int openAVCodec(void);
	void open() throw (AVException*);

    const char *av_get_codecid(enum AVCodecID);
    const char *av_get_field_order(enum AVFieldOrder);
    const char *av_get_media_type_string(enum AVMediaType );
    const char *av_get_colorrange(enum AVColorRange);
    const char *av_get_colorspace(enum AVColorSpace);
    void initMeta(AVFormatContext *);
	
public:
	Libav(std::string,int,int) throw (AVException*);
	~Libav();
	
	void dumpFormat(void);
	std::string getFilename(void);
	int decodeNextAudio(void) throw (AVException*);
	int decodeNextFrame(void) throw (AVException*);
    
    std::string getMetaKey(std::string);
    void dumpMeta();
    void setRequestSampleFormat(AVSampleFormat sf);
    
};
