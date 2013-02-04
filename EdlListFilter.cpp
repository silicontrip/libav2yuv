
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

void EdlListFilter::setFile(std::string filename, int st) throw (AVException*)
{
	
	std::ifstream fileContents(filename.c_str());
	std::vector<std::string> lines;
	std::string s;

	
	while (std::getline(fileContents, s)) { 
		entries.push_back(this->parseEDLEntry(s,st));
	}
}


AVObject * EdlListFilter::parseEDLEntry (std::string line, int st) throw (AVException*)
{
	std::string fileName;
	std::string mode;
	std::string transition;
	std::string tcIn;
	std::string tcOut;
	
	// grumble... it's delimited on a single space
	
	std::stringstream items(line);
	std::getline(items, fileName,' ');
	std::getline(items, mode,' ');
	std::getline(items, transition,' ');
	std::getline(items, tcIn,' ');
	std::getline(items, tcOut,' ');
	
	//	std::cerr << "fn:" << fileName << " m:" << mode << " t:" << transition << " in:" << tcIn << " out:" << tcOut << "\n";
	
	
	// more TODO
	Libav *entry;
	if ((st == AVMEDIA_TYPE_VIDEO) && this->hasVideo(mode))
	{
		std::cerr << "EDL Adding Video entry: " << fileName << "\n";
		entry = new Libav(fileName, AVMEDIA_TYPE_VIDEO, -1);
	}
	
	if ((st == AVMEDIA_TYPE_AUDIO) && this->hasAudio(mode))
	{
		std::cerr << "EDL Adding Audio entry: " << fileName << "\n";
		entry = new Libav(fileName, AVMEDIA_TYPE_AUDIO, -1);
	}
	entry->setInTimecode(tcIn);
	entry->setOutTimecode(tcOut);

	return entry;
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



