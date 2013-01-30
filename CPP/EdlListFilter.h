
#import "Libav.h"
#import "AVObject.h"

#import <iostream>
#import <string>
#import <list>
#import <fstream>

class EdlListFilter: public AVObject {

protected:
	std::list<Libav> entries;
	
	bool hasAudio(std::string); 
	bool hasVideo(std::string); 
	AVObject currentAV(void);

	
public:
	EdlListFilter();
	EdlListFilter(std::string, int);

	void setFile (std::string, int);
	
	int decodeNextFrame(void);
	int decodeNextAudio(void);

	void dumpFormat(void);
	
	// These are all supposed to be defined in AVObject
	AVRational getFrameRate(void) ;
	int getFrameRateNum(void); 
	int getFrameRateDen(void); 
	AVRational getSampleAspect(void);
	int getSampleAspectNum(void);
	int getSampleAspectDen(void);
	int getChromaSampling(void);
	bool getIsInterlaced(void);
	bool getInterlaceTopFieldFirst(void);
	int getHeight(void); 
	int getWidth(void);
	int getChromaHeight(void);
	int getChromaWidth(void);
	AVFrame * getAVFrame(void);
	int getSampleSize(void);
	int getSampleChannels(void);
	int getSampleFormat(void);
	int getSamplesPerSecond(void);
	double getSamplesPerFrame(void);

	
};