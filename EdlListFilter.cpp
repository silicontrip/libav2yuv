
#include "EdlListFilter.h"

EdlListFilter::EdlListFilter() 
{
	
	frameCounter = 0;
	
	this->setIn(-1);
	this->setOut(-1);
	
}

EdlListFilter::EdlListFilter(std::string filename, int st)  throw (AVException*)
{
	frameCounter = 0;
	
	this->setIn(-1);
	this->setOut(-1);
	this->setFile(filename,st);
	
}

struct edlEntry EdlListFilter::parseEDLEntry (std::string line, int st) 
{
	
	//	std::cerr << "fn:" << fileName << " m:" << mode << " t:" << transition << " in:" << tcIn << " out:" << tcOut << "\n";
	
	struct edlEntry entry;
	
	std::string mode;
	std::string transition;
	std::string tcIn;
	std::string tcOut;
	std::string duration;
	
	// grumble... it's delimited on a single space
	
	std::stringstream items(line);
	std::getline(items, entry.name,' ');
	std::getline(items, entry.channel,' ');
	std::getline(items, entry.transition,' ');
	
	// cut transition does not have a duration.
	if (entry.transition.compare("C")) {
		std::getline(items, entry.duration,' ');
	}
	
	std::getline(items, entry.sourceIn,' ');
	std::getline(items, entry.sourceOut,' ');
	
	return entry;
}

void EdlListFilter::setFile(std::string filename, int st) throw (AVException*)
{
	
	std::ifstream fileContents(filename.c_str());
	std::vector<std::string> lines;
	std::string s;

	std::vector<struct edlEntry> edlEntries;
	
	while (std::getline(fileContents, s)) { 
		edlEntries.push_back(this->parseEDLEntry(s,st));
	}
	
	std::cerr << "Size: " << edlEntries.size() << "\n";
	
	for (int i=0; i< edlEntries.size(); i++) 
	{
		struct edlEntry entry = edlEntries.at(i);
		
		struct edlEntry entry2;
		
		if (i+1 == edlEntries.size()) {
			entry2.name = "//BLACK"; // black generator
			entry2.sourceIn = "0";
		} else {
			entry2 = edlEntries.at(i+1);
		}
		
		if ((st == AVMEDIA_TYPE_VIDEO) && this->hasVideo(entry.channel)) {
			std::cerr << "EDL Adding Video entry: " << entry.name << "\n";
			
			AVObject *videoEntry;
			
			videoEntry = new Libav(entry.name, AVMEDIA_TYPE_VIDEO, -1);
			
			videoEntry->setInTimecode(entry.sourceIn);
			
			if (!entry.transition.compare("C")) {
				videoEntry->setOutTimecode(entry.sourceOut);
				entries.push_back(videoEntry);
			}
			if (!entry.transition.compare("D")) {
				
				
				int dur = atoi(entry.duration.c_str());

				std::cerr << "dissolve from: " << entry.name << " to: " << entry2.name << " for " << dur << " frames.\n";

				
				videoEntry->setOut(videoEntry->TCtoFrames(entry.sourceOut) - dur);
				entries.push_back(videoEntry);
				
				
				Libav *video1 = new Libav (entry.name, AVMEDIA_TYPE_VIDEO, -1);
				video1->setIn(video1->TCtoFrames(entry.sourceOut) - dur);
				video1->setOut(video1->TCtoFrames(entry.sourceOut));
				
				Libav *video2 = new Libav (entry2.name, AVMEDIA_TYPE_VIDEO, -1);
				video2->setIn(video2->TCtoFrames(entry2.sourceIn) );
				video2->setOut(video2->TCtoFrames(entry2.sourceIn) + dur );

				videoEntry = new DissolveTransition(video1,video2,dur);
				entries.push_back(videoEntry);

				videoEntry = new Libav(entry2.name, AVMEDIA_TYPE_VIDEO, -1);
								  
				videoEntry->setIn((videoEntry->TCtoFrames(entry2.sourceIn) + dur ));
				videoEntry->setOutTimecode(entry2.sourceOut);
  
				entries.push_back(videoEntry);
				i++;
				
			}
			
			
		}
		
		if ((st == AVMEDIA_TYPE_AUDIO) && this->hasAudio(entry.channel))
		{
			std::cerr << "EDL Adding Audio entry: " << entry.name << "\n";
			
			Libav *audioEntry;
			
			audioEntry = new Libav(entry.name, AVMEDIA_TYPE_AUDIO, -1);
			
			audioEntry->setInTimecode(entry.sourceIn);
			audioEntry->setOutTimecode(entry.sourceOut);
			
			
			entries.push_back(audioEntry);
		}
		
		std::cerr << "name: " << entry.name << " channel: " << entry.channel << " transition: " << entry.transition << " duration: " << entry.duration << " in: " << entry.sourceIn << " out: " << entry.sourceOut << "\n";
	}
	
}

bool EdlListFilter::hasAudio(std::string mode) 
{	
	return mode.find("A")!=std::string::npos ||  mode.find("a")!=std::string::npos ||
	mode.find("B")!=std::string::npos || mode.find("b")!=std::string::npos;	
}

bool EdlListFilter::hasVideo(std::string mode) 
{	
	return mode.find("V")!=std::string::npos ||  mode.find("v")!=std::string::npos ||
	mode.find("B")!=std::string::npos || mode.find("b")!=std::string::npos;	
}

int EdlListFilter::decodeNextFrame()
{

//	std::cerr << ">>EDL decodeNextFrame\n";
	
	if (this->compareRange(frameCounter) > 0) 
		return -1;
	
	int bytes;
	do {

		while ((bytes=this->currentAV()->decodeNextFrame()) < 0) {
			delete (this->currentAV());

			entries.pop_front();
			if (entries.empty())
				return -1;
		}
		frameCounter++;

	} while (this->compareRange(frameCounter)<0);
//	std::cerr << "<<EDL decodeNextFrame\n";

	return bytes;
}

int EdlListFilter::decodeNextAudio(void)
{
	
	// NSLog(@"edlListFilter decodeNextAudio sampleCounter %d entriesCount %d",sampleCounter,[entries count]);
	
	if (this->compareSamplesRange(sampleCounter) > 0)
		return -1;
	
	int samples;
	do {
		while ((samples=this->currentAV()->decodeNextAudio()) < 0) {
			delete (this->currentAV());
			entries.pop_front();
			if (entries.empty())
				return -1;
		}
		sampleCounter +=samples; 
		
	} while (this->compareSamplesRange(sampleCounter) < 0);
	return samples;
}


void EdlListFilter::dumpFormat(void)
{
	for (std::list<AVObject *>::iterator o=entries.begin(); o != entries.end(); ++o)
		(*o)->dumpFormat();
	std::cerr<<"EDL List AVObject\n";

}

AVObject * EdlListFilter::currentAV(void)
{
	return entries.front();
}

AVRational EdlListFilter::getFrameRate(void) 
{ 
	return this->currentAV()->getFrameRate();
}

int EdlListFilter::getFrameRateNum(void) 
{
	return this->currentAV()->getFrameRateNum(); 
}
int EdlListFilter::getFrameRateDen(void) 
{ 
	return this->currentAV()->getFrameRateDen(); 
}
AVRational EdlListFilter::getSampleAspect(void) 
{ 
	return this->currentAV()->getSampleAspect(); 
}
int EdlListFilter::getSampleAspectNum(void) 
{ 
	return this->currentAV()->getSampleAspectNum(); 
}
int EdlListFilter::getSampleAspectDen(void) 
{ 
	return this->currentAV()->getSampleAspectDen(); 
}
PixelFormat EdlListFilter::getChromaSampling(void) 
{ 
	return this->currentAV()->getChromaSampling(); 
}
bool EdlListFilter::getIsInterlaced(void) 
{ 
	return this->currentAV()->getIsInterlaced(); 
}
bool EdlListFilter::getInterlaceTopFieldFirst(void) 
{
	return this->currentAV()->getInterlaceTopFieldFirst(); 
}
int EdlListFilter::getHeight(void) 
{ 
	return this->currentAV()->getHeight(); 
}
int EdlListFilter::getWidth(void)
{ 
	return this->currentAV()->getWidth(); 
}
int EdlListFilter::getChromaHeight(void)
{ 
	return this->currentAV()->getChromaHeight(); 
}
int EdlListFilter::getChromaWidth(void)
{ 
	return this->currentAV()->getChromaWidth(); 
}
AVFrame * EdlListFilter::getAVFrame(void) 
{ 
	return this->currentAV()->getAVFrame(); 
}
int EdlListFilter::getSampleSize(void)
{ 
	return this->currentAV()->getSampleSize(); 
}
int EdlListFilter::getSampleChannels(void)
{ 
	return this->currentAV()->getSampleChannels(); 
}
AVSampleFormat EdlListFilter::getSampleFormat(void)
{ 
	return this->currentAV()->getSampleFormat(); 
}
int EdlListFilter::getSamplesPerSecond(void)
{ 
	return this->currentAV()->getSamplesPerSecond();
}
double EdlListFilter::getSamplesPerFrame(void)
{ 
	return this->currentAV()->getSamplesPerFrame(); 
}



