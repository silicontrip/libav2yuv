
#import "Libav.h"
#import "Libyuv.h"
#import "Libav2yuvArguments.h"
#import "AVObject.h"
#import "ChromaFilter.h"
#import "EdlListFilter.h"
#import "LibavWaveWriter.h"


// wave writer
void processAudio (Libav2yuvArguments options, std::vector<AVObject *> edlList)
{
	
	AVObject *lav = edlList.front();
	
	//libavWaveWriter *nfh=[[libavWaveWriter alloc] init];
	
	//NSLog(@"libavWaveWriter initWithFormat");
	
	LibavWaveWriter nfh;
	nfh.setSampleFormat(lav->getSampleFormat());
	nfh.setSampleChannels(lav->getSampleChannels());
	
	nfh.setSamplesPerSecond(lav->getSamplesPerSecond());
	
	if (options.hasOutFile()){
		nfh.setOutputFilename(options.getOutFile());
	} 
	
	
	// need to decode the first frame to get the Samples per second.
	
	if (lav->decodeNextAudio() >= 0) {
		
		nfh.setSamplesPerSecond(lav->getSamplesPerSecond());
		
		nfh.writeHeader();
		nfh.writeFrameData(lav->getAVFrame());
		
		std::vector<AVObject *>::iterator audio;
		for (audio=edlList.begin(); audio != edlList.end(); ++audio) 
		{
			
			lav = *audio;
			//for (AVObject *audio in edlList) {
			
			//	if (audio != nil) {
			lav->dumpFormat();
			
			
			int sampleSize = lav->getSampleSize() * lav->getSampleChannels();
			
			// 	std::cerr<<"sample size: " << lav->getSampleSize() <<"x"<< lav->getSampleChannels() << " = " << sampleSize << "\n";
			
			while (lav->decodeNextAudio() >=0)
			{
				nfh.writeFrameData(lav->getAVFrame());
				
				//	[audio freeAVFrame];
			}
			//NSLog(@"audio release");
			
			delete lav;
		}
	} else {
		std::cerr<< "Couldn't initialise audio\n";
	}
	
	//NSLog(@"nfh release");
	
	// close file
	// delete nfh;	
	
}

void processVideo (Libav2yuvArguments options, std::vector<AVObject *> edlList)
{
	
	AVObject *lav = edlList.front();
	
	lav->dumpFormat();
	
	//	NSLog(@"%dx%d by %d:%d at %d:%d",[lav getWidth],[lav getHeight],[lav getSampleAspectNum],[lav getSampleAspectDen],[lav getFrameRateNum],[lav getFrameRateDen]);
	
	Libyuv yuv(lav->getWidth(),
			   lav->getHeight(),
			   lav->getSampleAspect(),
			   lav->getFrameRate(),
			   lav->getChromaSampling());
	
	if (options.getExtensions())
	{
		yuv.setExtensions(1);
	}
	if (options.hasAspect()) {
		yuv.setSampleAspect(options.getAspect());
	}
	if (options.hasFrameRate()) {
		yuv.setFrameRate(options.getFrameRate());
	}
	if (options.hasChroma()){
		yuv.setChromaSampling(options.getChroma());
	}		
	if (options.hasOutFile()){
		yuv.setOutputFilename(options.getOutFile());
	}
	// need to decode the first frame to get the interlace type
	// NSLog(@"lav decodeNextFrame");
	
	lav->decodeNextFrame();
	//	 NSLog(@"yuv setYUVFrameDataWithAVFrame");
	
	yuv.setYUVFrameDataWithAVFrame(lav->getAVFrame());
	//	NSLog(@"yuv setInterlaceAndOrder");
	
	yuv.setInterlaceAndOrder(lav->getIsInterlaced(), lav->getInterlaceTopFieldFirst());
	//interlace flag is not available until the first frame is decoded.
	//need to get the interlace flags before this. So we can use a generator.
	//	 NSLog(@"yuv writeHeader");
	
	yuv.writeHeader();
	yuv.write();
	// [lav release];
	
	for (std::vector<AVObject *>::iterator video=edlList.begin(); video != edlList.end(); ++video)
	{
		
		lav = *video;
		
		//for (AVObject *video in edlList) {
		//	if (video != nil) {
		
		while (lav->decodeNextFrame() >=0)
		{
			yuv.setYUVFrameDataWithAVFrame(lav->getAVFrame());
			yuv.write();
		}
		delete *video;
	}
	
	// delete yuv;	
	
}

int main(int argc, char *argv[])
{
	
	std::vector<AVObject *> edlList;
	
	//pool = [[NSAutoreleasePool alloc] init];
	//edlList = [NSMutableArray arrayWithCapacity:1];
	int streamMode = AVMEDIA_TYPE_VIDEO;
	
	Libav2yuvArguments options(argc,argv);
	
	if (options.getAudio()) {
		// std::cerr << "setting to audio\n";
		streamMode = AVMEDIA_TYPE_AUDIO;
	}	
	// std::cerr<<"scan arguments\n";
	
	std::vector<std::string> args = options.getArguments();
	
	if (args.size() > 0) {
		//std::cerr<<"argument size " << options.getArguments().size() << "\n";
		std::vector<std::string>::iterator argument = args.begin();
		
		for (; argument != args.end(); ++argument)
		{	
			
			// std::cerr<<  "argument name: " << *argument << "\n";
			
			std::string name (*argument);
			
			// for (NSString *argument in [options getArguments]) {
			AVObject *lav;
			//std::cerr << "extension: " << argument->substr(argument->size()-4,4) << "\n";
			if (name.substr(name.size()-4,4) == ".edl") {
				lav = new EdlListFilter(name, streamMode);
			} else  {	
				lav = new Libav(name,streamMode,-1);
			}
			
			// if range
			if (options.hasRange()) {
				lav->setInOutTimecode(options.getRange());
			}
			
			//if (lav != nil) 
			//	{
			if (options.getConvert()) 
			{
				ChromaFilter * chromaConverter = new ChromaFilter(lav,options.getChroma());
				//	if (chromaConverter != nil) {
				//[edlList addObject:chromaConverter];
				edlList.push_back(chromaConverter);
				//	}
			} else {
				edlList.push_back(lav);
			}
			
		}	
	}
	// 	NSLog(@"EDL list count: %d",[edlList count]);
	
	if (edlList.size() > 0 ) 
	{
		if (streamMode == AVMEDIA_TYPE_VIDEO) {
			processVideo(options,edlList);
		} else {
			// decode audio
			processAudio(options,edlList);
		}
	}
	
	
}
