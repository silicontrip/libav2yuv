
/*
 http://www.edlmax.com/EdlMaxHelp/Edl/maxguide.html
 
 Format for EDL file
 
 Here are several examples (CMX format):
 
 Cut to "BLACK" on video and A1 & A2:
 
 Edit # Reel Name  Channel Trans      Dur      Source IN          Source OUT          Record IN         Record OUT
 002    BL  AA/V  C        00:00:00:00 00:00:10:00 00:59:45:00 00:59:55:00
 75 frame dissolve from reel 123 to reel 123B on video and A1:
 
 Edit # Reel Name  Channel Trans      Dur      Source IN          Source OUT          Record IN         Record OUT
 105 123    B     C        03:05:57:17 03:05:57:17 01:00:21:20 01:00:21:20
 105 123B   B     D    075 03:15:33:09 03:15:35:24 01:00:21:20 01:00:24:05
 30 frame wipe (pattern 001) from reel 002 to reel 003 on video only:
 
 Edit # Reel Name  Channel Trans      Dur      Source IN          Source OUT          Record IN         Record OUT
 002 002    V     C        02:00:10:00 02:00:10:00 01:00:13:18 01:00:13:18 
 002 003    V     W001 030 03:00:00:00 03:00:10:00 01:00:13:18 01:00:23:18 
 "Split" edit to reel 021, first to video-only, then video and A1, with two notes:
 
 Edit # Reel Name  Channel Trans      Dur      Source IN          Source OUT          Record IN         Record OUT
 SPLIT:    AUDIO DELAY=  00:00:00:06
 053 021    V     C        01:21:05:19 01:21:06:26 01:00:51:11 01:00:52:18
 053 021    A     C        01:21:05:25 01:21:06:26 01:00:51:17 01:00:52:18
 HERES A NOTE
 THERE CAN BE MORE
 KEY- Background-reel 131, Foreground-reel 051, with freeze on reel 131, with two notes:
 
 Edit # Reel Name  Channel Trans      Dur      Source IN          Source OUT          Record IN         Record OUT
 037 131    V     K B      10:57:40:22 10:57:41:11 01:00:36:08 01:00:36:27
 037 051    V     K O  000 01:07:39:05 01:07:39:24 01:00:36:08 01:00:36:27
 M2  131          000.0                10:57:41:00
 
 
 This sample is taken from an actual CMX EDL file.
 
 This EDL Filter parser format is in the form:
 
 # <filename|tag>  <Channel>  <TransitionType>[num]  [duration]  [srcIn]  [srcOut]  [recIn]  [recOut]
 /Users/d332027/Movies/TheFaceOfTheEnemy/the_face_of_the_enemy_01.mpg VA C 0:0:0:0 0:4:9:0
 /Users/d332027/Movies/TheFaceOfTheEnemy/the_face_of_the_enemy_02.mpg VA C 0:0:0:0 0:3:39:0
 /Users/d332027/Movies/TheFaceOfTheEnemy/the_face_of_the_enemy_03.mpg VA C 0:0:0:0 0:2:38:0
 /Users/d332027/Movies/TheFaceOfTheEnemy/the_face_of_the_enemy_04.mpg VA C 0:0:0:0 0:3:9:0
 /Users/d332027/Movies/TheFaceOfTheEnemy/the_face_of_the_enemy_05.mpg VA C 0:0:0:0 0:4:39:0
 /Users/d332027/Movies/TheFaceOfTheEnemy/the_face_of_the_enemy_06.mpg VA C 0:0:0:0 0:4:9:0
 /Users/d332027/Movies/TheFaceOfTheEnemy/the_face_of_the_enemy_07.mpg VA C 0:0:0:0 0:2:9:0
 /Users/d332027/Movies/TheFaceOfTheEnemy/the_face_of_the_enemy_08.mpg VA C 0:0:0:0 0:3:9:0
 /Users/d332027/Movies/TheFaceOfTheEnemy/the_face_of_the_enemy_09.mpg VA C 0:0:0:0 0:3:38:15
 /Users/d332027/Movies/TheFaceOfTheEnemy/the_face_of_the_enemy_10.mpg VA C 0:0:0:0 0:5:46:0
 
 The edit number and reel name is replaced by a filename.
 Channel works in the same way.  However A refers to both audio channels, left and right.
 
 Transition Type. Currently only 'C' (for cut) is supported.
 Source in and Source out work in the same way.  Many video files do not support time code. So the first frame of the file is considered 0:0:0:0
 
 Record in and record out are not used.
 
 */


#import "Libav.h"
#import "AVObject.h"

#import <iostream>
#import <string>
#import <list>
#import <fstream>

class EdlListFilter: public AVObject {

protected:
	std::list<AVObject *> entries;
	
	bool hasAudio(std::string); 
	bool hasVideo(std::string); 
	AVObject * currentAV(void);

	
public:
	EdlListFilter();
	EdlListFilter(std::string, int) throw (AVException*);

	void setFile (std::string, int) throw (AVException*);
	
	AVObject* parseEDLEntry(std::string, int) throw (AVException*);

	
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
	PixelFormat getChromaSampling(void);
	bool getIsInterlaced(void);
	bool getInterlaceTopFieldFirst(void);
	int getHeight(void); 
	int getWidth(void);
	int getChromaHeight(void);
	int getChromaWidth(void);
	AVFrame * getAVFrame(void);
	int getSampleSize(void);
	int getSampleChannels(void);
	AVSampleFormat getSampleFormat(void);
	int getSamplesPerSecond(void);
	double getSamplesPerFrame(void);

	
};