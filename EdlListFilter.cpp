
#include "EdlListFilter.h"

EdlListFilter::EdlListFilter() 
{
	
	frameCounter = 0;
	
	this->setIn(-1);
	this->setOut(-1);
	
}

EdlListFilter::~EdlListFilter() 
{
	pFrame = NULL;
}

EdlListFilter::EdlListFilter(std::string filename, int st)  throw (AVException*)
{
	frameCounter = 0;
	
	this->setIn(-1);
	this->setOut(-1);
	this->setFile(filename,st);
	
}

struct edlEntry EdlListFilter::parseEDLEntry (std::string line, int st) throw (AVException *)
{
	
	//	std::cerr << "fn:" << fileName << " m:" << mode << " t:" << transition << " in:" << tcIn << " out:" << tcOut << "\n";
	
	struct edlEntry entry;
	
	// grumble... it's delimited on a single space
	// multiple spaces are treated as multiple delimiters.
	
	entry.duration = "0";
	
	std::stringstream items(line);
	std::getline(items, entry.name,' ');
	std::getline(items, entry.channel,' ');
	std::getline(items, entry.transition,' ');
	
	// cut transition does not have a duration.
	if (entry.transition.compare("C")) { // if not Cut
		std::getline(items, entry.duration,' ');
	}
	
	std::getline(items, entry.sourceIn,' ');
	std::getline(items, entry.sourceOut,' ');
	
	if (entry.name.size() == 0) 
		throw new AVException("Invalid EDL: no name",EDL_PARSER_ERROR);
	
	if (entry.channel.size() == 0) 
		throw new AVException("Invalid EDL: no channel",EDL_PARSER_ERROR);

	if (entry.transition.size() == 0) 
		throw new AVException("Invalid EDL: no transition",EDL_PARSER_ERROR);

	if (entry.duration.size() == 0) 
		throw new AVException("Invalid EDL: no duration for non cut transition",EDL_PARSER_ERROR);

	if (entry.sourceIn.size() == 0) 
		throw new AVException("Invalid EDL: no in",EDL_PARSER_ERROR);

	if (entry.sourceOut.size() == 0) 
		throw new AVException("Invalid EDL: no out",EDL_PARSER_ERROR);

	
	return entry;
}

AVObject * EdlListFilter::videoFactory (std::string filename) {

	AVObject * video;
	
	// Arg! black generator needs a framerate.
	if (filename.compare("//BLACK") == 0) 
	{
		video = new AVObject();
		
		if (hasWidth())
			video->setWidth(getWidth());
		if (hasHeight()) 
			video->setHeight(getHeight());
		if (hasFrameRate())
			video->setFrameRate(getFrameRate());
		if (hasChromaSampling())
			video->setChromaSampling(getChromaSampling());
		if (hasSampleAspect())
			video->setSampleAspect(getSampleAspect());
		
		video->allocFrame();
		
	} else {
		video =  new Libav(filename, AVMEDIA_TYPE_VIDEO, -1);
	}
	
	
	return video;
}

void EdlListFilter::validateList(std::vector<struct edlEntry> edlEntries, int st) throw (AVException *)
{
	
	for (int i=0; i< edlEntries.size(); i++) 
	{
		struct edlEntry entry = edlEntries.at(i);
		
		if ((st == AVMEDIA_TYPE_VIDEO) && this->hasVideo(entry.channel)) 
		{
			AVObject *videoEntry;
			videoEntry = this->videoFactory(entry.name);
			
			if (getWidth() == 0) 
				this->setWidth(videoEntry->getWidth());
			else
				if (videoEntry->getWidth() != 0 && getWidth() != videoEntry->getWidth())
					throw new AVException("Inconsistent video Widths",EDL_PARSER_ERROR);
			
			if (getHeight() == 0) 
				setHeight(videoEntry->getHeight());
			else
				if (videoEntry->getHeight() != 0 && getHeight() != videoEntry->getHeight())
					throw new AVException("Inconsistent video Heights",EDL_PARSER_ERROR);
			
			if (getFrameRateNum()==0 && getFrameRateDen()==0) 
				setFrameRate(videoEntry->getFrameRate());
			else 
				if (videoEntry->getFrameRateNum() !=0 && 
					videoEntry->getFrameRateDen() !=0 && 
					getFrameRateNum() !=videoEntry->getFrameRateNum() && 
					getFrameRateDen() != videoEntry->getFrameRateDen())
					throw new AVException("Inconsistent video Framerates",EDL_PARSER_ERROR);
			
			if (getSampleAspectNum()==0 && getSampleAspectDen()==0) 
				setSampleAspect(videoEntry->getSampleAspect());
			else 
				if (videoEntry->getSampleAspectNum() !=0 && videoEntry->getSampleAspectDen()!=0 && 
					getSampleAspectNum() !=videoEntry->getSampleAspectNum() && getSampleAspectDen() != videoEntry->getSampleAspectDen())
					throw new AVException("Inconsistent video Aspect Ratio (this is probably minor, I may ignore this in future)",EDL_PARSER_ERROR);
			
			if (getChromaSampling() == PIX_FMT_NONE)
				setChromaSampling(videoEntry->getChromaSampling());
			else
				if (videoEntry->getChromaSampling() !=PIX_FMT_NONE && getChromaSampling() != videoEntry->getChromaSampling())
					throw new AVException("Inconsistent video Chroma subsampling (sometimes this can be ignored)",EDL_PARSER_ERROR);
			
			delete videoEntry;
			
		}
		if ((st == AVMEDIA_TYPE_AUDIO) && this->hasAudio(entry.channel))
		{
			AVObject *audioEntry;
			audioEntry = new Libav(entry.name, AVMEDIA_TYPE_AUDIO, -1);
			// check consistency
			delete audioEntry;
		}
	}
	
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
	
	//  validate the entries.
			
	this->validateList(edlEntries,st);
	
	for (int i=0; i< edlEntries.size(); i++) 
	{
		struct edlEntry entry = edlEntries.at(i);
		
		if ((st == AVMEDIA_TYPE_VIDEO) && this->hasVideo(entry.channel)) {
			//std::cerr << "EDL Adding Video entry: " << entry.name << "\n";
			
			
			// This behaviour is incorrect.
			// transitions occur at the beginning of the current edit
			// and after the out point of the previous 
			
			
			// add previous transition duration to IN
/*
			if (i>0) 
			{
				
				struct edlEntry previous = edlEntries.at(i-1);
				std::stringstream ssdur(previous.duration,std::stringstream::in);
				int duration;
				ssdur >> duration;
				
				int fr = this->TCtoFrames(entry.sourceIn) + duration;
				//std::cerr << "in frame: " << fr << "\n";
				
				entry.sourceIn = this->FramesToTC(fr);

			}
*/			
			AVObject *videoEntry;
			
				
			if (!entry.transition.compare("C")) {
				videoEntry = this->videoFactory(entry.name);
				
				videoEntry->setInTimecode(entry.sourceIn);
				videoEntry->setOutTimecode(entry.sourceOut);
				
				entries.push_back(videoEntry);
			}
			if (!entry.transition.compare("D")) {
				
				struct edlEntry previous;
				
				if (i==0) {
					previous.name = "//BLACK"; // black generator
					previous.sourceIn = "0";
					previous.sourceOut = "0";
				} else {
					previous = edlEntries.at(i-1);
				}
				
				int dur = atoi(entry.duration.c_str());

				int transitionOut = this->TCtoFrames(previous.sourceOut);
				int transitionIn = this->TCtoFrames(entry.sourceIn);
				
				// transition comes first.
				
				AVObject *video1 = this->videoFactory(previous.name);
				video1->setIn(transitionOut);
				video1->setOut(transitionOut + dur);
				
				// create transition
				AVObject *video2 = this->videoFactory(entry.name);
				video2->setIn(transitionIn);
				video2->setOut(transitionIn + dur);
				
				
				AVObject * dissolveEntry = new DissolveTransition(video1,video2,dur);
				
				entries.push_back(dissolveEntry);
				
				videoEntry = this->videoFactory(entry.name);
				videoEntry->setIn(transitionIn + dur);
				videoEntry->setOutTimecode(entry.sourceOut);

				entries.push_back(videoEntry);

				
		//	std::cerr << "dissolve from: " << entry.name << " to: " << entry2.name << " for " << dur << " frames.\n";

				//create transition FROM video 
				// change out point 

			}
			
			
		}
		
		if ((st == AVMEDIA_TYPE_AUDIO) && this->hasAudio(entry.channel))
		{
		//	std::cerr << "EDL Adding Audio entry: " << entry.name << "\n";
			
			Libav *audioEntry;
			
			audioEntry = new Libav(entry.name, AVMEDIA_TYPE_AUDIO, -1);
			
			audioEntry->setInTimecode(entry.sourceIn);
			audioEntry->setOutTimecode(entry.sourceOut);
			
			
			entries.push_back(audioEntry);
		}
		
	//	std::cerr << "name: " << entry.name << " channel: " << entry.channel << " transition: " << entry.transition << " duration: " << entry.duration << " in: " << entry.sourceIn << " out: " << entry.sourceOut << "\n";
	}
	
//	this->currentAV()->dumpFormat();

	pFrame = this->currentAV()->getAVFrame();

	
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

// std::cerr << ">>EDL decodeNextFrame\n";
	
	if (this->compareRange(frameCounter) > 0) 
		return -1;
	
	int bytes;
	do {

		while ((bytes=this->currentAV()->decodeNextFrame()) < 0) {
			delete (this->currentAV());

			entries.pop_front();
			if (entries.empty()) {
				pFrame = NULL;
				return -1;
			}
			//this->currentAV()->dumpFormat();
			pFrame = this->currentAV()->getAVFrame();
			
		}
	//	std::cerr << "  EDL decodeNextFrame decoded.\n";

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
	
	std::cerr << "\n";
	std::cerr << "EDL List AVObject: "  << this->getWidth() << "x" << this->getHeight() << " FPS:" << this->getFrameRateAsString() << " chroma: " << this->getChromaSampling()  << "\n";

	int count = 1;
	
	for (std::list<AVObject *>::iterator o=entries.begin(); o != entries.end(); ++o)
	{
		std::cerr << count++ << ". ";
		(*o)->dumpFormat();
	}
	//std::cerr << "EDL List AVObject: "  << this->getWidth() << "x" << this->getHeight() << " FPS:" << this->getFrameRateAsString()  << "\n";
	

}

AVObject * EdlListFilter::currentAV(void)
{
	return entries.front();
}

/*
AVFrame * EdlListFilter::getAVFrame(void) 
{ 
	return this->currentAV()->getAVFrame(); 
}
*/

/*
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
*/


