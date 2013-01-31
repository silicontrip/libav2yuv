extern "C" {
#include <yuv4mpeg.h>
#include <libavcodec/avcodec.h>
#include <getopt.h>
}

#include <string>
#include <vector>
#include <iostream>

#define PAL "PAL"
#define PAL_WIDE "PAL_WIDE"
#define NTSC "NTSC"
#define NTSC_WIDE "NTSC_WIDE"


class Libav2yuvArguments {

private:
	bool extensions;
	bool audio;
	bool convert;
	
	int interlace;
	y4m_ratio_t frameRate;
	std::string format;
	y4m_ratio_t aspect;
	int chroma;
	std::string outFile;
	std::string range;
	
	std::vector<std::string> arguments;
	int stream;
	
public:
	Libav2yuvArguments();
	Libav2yuvArguments(int, char**);

	void usage(void);
	void chromaModes(void);
	
	std::vector<std::string> getArguments(void) { return arguments; }
	int getInterlace(void) { return interlace; }
	int getChroma(void) { return chroma; }
	y4m_ratio_t getFrameRate(void) { return frameRate; }
	y4m_ratio_t getAspect(void) { return aspect; }
	std::string getFormat(void) { return format; }
	std::string getRange(void) { return range; }

	std::string getOutFile(void) { return outFile; }
	bool getExtensions(void) { return extensions; }
	bool getAudio(void) { return audio; }
	bool getConvert(void) { return convert; }
	
	bool hasInterlace(void) { return interlace != -1; }
	bool hasChroma(void) { return chroma != -1; }
	bool hasFrameRate(void) { return frameRate.d != 0; }
	bool hasAspect(void) { return aspect.d != 0; }
	bool hasOutFile(void) { return !outFile.empty(); }
	bool hasFormat(void) { return !format.empty(); }
	bool hasRange(void) { return !range.empty(); }
	bool hasStream(void) { return stream != -1; }
	
	
};