
#include "Libyuv.h"


void Libyuv::init(void) throw (AVException*)
{
	y4m_init_stream_info (&yuvStreamInfo);
	y4m_init_frame_info(&yuvFrameInfo);
    
	fileHeaderWritten = false;
	frameData = NULL;
	frameData = (uint8_t **)malloc(sizeof (uint8_t *) * 3);
	if (!frameData)
		throw new AVException("unable to allocate yuv buffer",MEMORY_ALLOCATION_ERROR);
	frameData[0]=NULL;
	frameData[1]=NULL;
	frameData[2]=NULL;
	
	this->setOutputFd(1); // STDOUT
	this->setInputFd(0); // STDIN
}

Libyuv::Libyuv() throw (AVException*)
{
	this->init();
	// NSLog(@"> libyuv init");
	//NSLog(@"< libyuv init");
}

/*
 Libyuv::Libyuv(int w, int h, AVRational sa, AVRational fr, PixelFormat pf) throw (AVException*)
 {
 
 this->init();
 this->setWidth(w);
 this->setHeight(h);
 this->setSampleAspectAVRational(sa);
 this->setFrameRateAVRational(fr);
 this->setChromaSamplingFromAV(pf);
 
 this->allocFrameData();
 
 
 }
 */
Libyuv::Libyuv(int w, int h, y4m_ratio_t sa, y4m_ratio_t fr, int pf) throw (AVException*)
{
    
	this->init();
	this->setWidth(w);
	this->setHeight(h);
	this->setSampleAspect(sa);
	this->setFrameRate(fr);
	this->setChromaSampling(pf);
	
	this->allocFrameData();
	
	
}


Libyuv::~Libyuv()
{
	y4m_fini_frame_info(&yuvFrameInfo);
	y4m_fini_stream_info(&yuvStreamInfo);
	close (fdOut);
	close (fdIn);
	this->deallocFrameData();
}


void Libyuv::allocFrameData(void) throw (AVException*)
{
	
	int fs,cfs;
	
	fs = y4m_si_get_plane_length(&yuvStreamInfo,0);
	cfs = y4m_si_get_plane_length(&yuvStreamInfo,1);
	
	frameData[0] = (uint8_t *)malloc( fs );
	if (!frameData[0])
		throw new AVException("unable to allocate yuv buffer data",MEMORY_ALLOCATION_ERROR);
    
	frameData[1] = (uint8_t *)malloc( cfs);
	if (!frameData[1]){
		free (frameData[0]);
		throw new AVException("unable to allocate yuv buffer data",MEMORY_ALLOCATION_ERROR);
	}
	frameData[2] = (uint8_t *)malloc( cfs);
	if (!frameData[2]){
		free (frameData[0]);
		free (frameData[1]);
		throw new AVException("unable to allocate yuv buffer data",MEMORY_ALLOCATION_ERROR);
	}
	
}

void Libyuv::setExtensions(int ext)
{
	y4m_accept_extensions(ext);
}

int Libyuv::setOutputFilename(std::string filename) throw (AVException*)
{
	
	fdOut = open(filename.c_str(),O_WRONLY|O_CREAT,0644);
	if (fdOut <0)
		throw new AVException("Unable to open output file",IO_ERROR);
	return fdOut;
	
}

void Libyuv::setOutputFd(int fd)
{
	fdOut = fd;
}

void Libyuv::setInputFd(int fd)
{
    fdIn = fd;
}

void Libyuv::setWidth(int w)
{
	y4m_si_set_width(&yuvStreamInfo, w);
}
void Libyuv::setHeight(int h)
{
	y4m_si_set_height(&yuvStreamInfo, h);
}

void Libyuv::setInterlacing(int i)
{
	y4m_si_set_interlace(&yuvStreamInfo,i);
}
/*
void Libyuv::setInterlaceAndOrder(int i, int tff)
{
	
	if (i) {
		if (tff) {
			this->setInterlacing(Y4M_ILACE_TOP_FIRST);
		} else {
			this->setInterlacing(Y4M_ILACE_BOTTOM_FIRST);
		}
	} else {
		this->setInterlacing(Y4M_ILACE_NONE);
	}
}
*/
void Libyuv::setChromaSampling (int ch)
{
	if (ch != Y4M_CHROMA_420MPEG2 && ch != Y4M_CHROMA_420JPEG)
		this->setExtensions(1);
	
	y4m_si_set_chroma(&yuvStreamInfo,ch);
}

/*
 void Libyuv::setChromaSamplingFromAV(PixelFormat pix_fmt) throw (AVException*)
 {
 switch (pix_fmt)
 {
 case PIX_FMT_YUV420P: this->setChromaSampling(Y4M_CHROMA_420MPEG2); break;
 case PIX_FMT_YUVJ422P:
 case PIX_FMT_YUV422P: this->setChromaSampling(Y4M_CHROMA_422); break;
 case PIX_FMT_YUVJ444P:
 case PIX_FMT_YUV444P: this->setChromaSampling(Y4M_CHROMA_444); break;
 case PIX_FMT_YUV411P: this->setChromaSampling(Y4M_CHROMA_411); break;
 case PIX_FMT_YUVJ420P: this->setChromaSampling(Y4M_CHROMA_420JPEG); break;
 default:
 throw new AVException("unsupported y4m chroma",UNSUPPORTED_CHROMA);
 //		std::cerr<<"LIBYUV: Unsupported Chroma: " << pix_fmt << "\n";
 break;
 }
 }
 */

void Libyuv::setFrameRate(y4m_ratio_t fr)
{
	
	if (fr.d==0 && fr.n==0) {
		fr.d = 1;
		fr.n=1;
	}
	
	y4m_si_set_framerate(&yuvStreamInfo, fr);
}

/*
 void Libyuv::setFrameRateAVRational(AVRational rational)
 {
 y4m_ratio_t fr;
 
 fr.d = rational.den;
 fr.n = rational.num;
 this->setFrameRate(fr);
 }
 */
void Libyuv::setSampleAspect(y4m_ratio_t sa)
{
	
	// check for invalid aspect ratio
	if (sa.d==0 && sa.n==0) {
		sa.d = 1;
		sa.n=1;
	}
	
	y4m_si_set_sampleaspect(&yuvStreamInfo, sa);
}

/*
 void Libyuv::setSampleAspectAVRational(AVRational rational)
 {
 y4m_ratio_t sa;
 
 sa.d = rational.den;
 sa.n = rational.num;
 this->setSampleAspect(sa);
 }
 */
int Libyuv::getHeight(void)
{
	return y4m_si_get_plane_height(&yuvStreamInfo,0);
}
int Libyuv::getWidth(void)
{
	return y4m_si_get_plane_width(&yuvStreamInfo,0);
}
int Libyuv::getChromaHeight(void)
{
	return y4m_si_get_plane_height(&yuvStreamInfo,1);
}
int Libyuv::getChromaWidth(void)
{
	return y4m_si_get_plane_width(&yuvStreamInfo,1);
}


// ideally this should be in a support class
// Libyuv should know nothing about AVFrame
void Libyuv::setYUVFrameDataWithAVFrame(AVFrame *pFrame)
{
	
	int y;
	int h = this->getHeight();
	int w = this->getWidth();
	int ch = this->getChromaHeight();
	int cw = this->getChromaWidth();
    
    int cch = 0,cchh = 0;
    int ls0 =0, ls1=0;
    
	//fprintf(stderr, " memcpy0 %d %d %x %x\n",w,pFrame->linesize[0],frameData[0],pFrame->data[0]);
	//std::cerr<<"memcpy0 " << w << " " << pFrame->linesize[0] << " " <<  frameData[0] << " " << std::hex  << pFrame->data[0] << "\n";
    
    // check that AVFrame matches the YUVFrame
    
    if (pFrame->width >= w && pFrame->height >= h)
    {
        
        
        for (y=0; y<h; y++) {
            
            memcpy(frameData[0]+cch,(pFrame->data[0])+ls0,w);
            cch += w;
            ls0 += pFrame->linesize[0];
            if (y<ch) {
                memcpy(frameData[1]+cchh,(pFrame->data[1])+ls1,cw);
                memcpy(frameData[2]+cchh,(pFrame->data[2])+ls1,cw);
                cchh += cw;
                ls1 += pFrame->linesize[1];
            }
        }
    }
}

uint8_t **Libyuv::getYUVFrame(void)
{
    return frameData;
}


int Libyuv::writeHeader(void) throw (AVException*)
{
	
	int write_error_code = y4m_write_stream_header(fdOut, &yuvStreamInfo);
    
    //  std::cerr<< "Error: " << write_error_code << " perr:" << strerror(errno) << " fdout: " << fdOut << "\n";
    
	if (write_error_code == Y4M_OK)
		fileHeaderWritten = true;
	else
		throw new AVException ("unable to write Y4M header",IO_ERROR);
	
	return write_error_code;
}

int Libyuv::readHeader(void) throw (AVException*)
{
	
	int read_error_code = y4m_read_stream_header(fdIn, &yuvStreamInfo);
    //  std::cerr<< "Error: " << read_error_code << " perr:" << strerror(errno) << "\n";
    
    
	
    if (read_error_code == Y4M_OK)
		fileHeaderRead = true;
	else
		throw new AVException ("unable to read Y4M header: ",IO_ERROR);
	
	return read_error_code;
}

int Libyuv::read(void) throw (AVException *)
{
    
	y4m_init_frame_info( &yuvFrameInfo );
    
    int read_error_code = y4m_read_frame(fdIn, &yuvStreamInfo, &yuvFrameInfo,frameData );
    
    if (read_error_code != Y4M_OK)
		throw new AVException ("unable to read Y4M frame",IO_ERROR);
	
	return read_error_code;
    
}

int Libyuv::write(void) throw (AVException*)
{
	int write_error_code;
	
	write_error_code = y4m_write_frame(fdOut, &yuvStreamInfo, &yuvFrameInfo, frameData);
	if (write_error_code != Y4M_OK)
		throw new AVException ("unable to write Y4M data",IO_ERROR);
    
	return write_error_code;
}

void Libyuv::deallocFrameData(void)
{
	if (frameData != NULL)
	{
		if (frameData[0] != NULL)
			free(frameData[0]);
		if (frameData[1] != NULL)
			free(frameData[1]);
		if (frameData[2] != NULL)
			free(frameData[2]);
        
		free(frameData);
	}
}

void Libyuv::copyStreamInfo(Libyuv *in)
{
    y4m_copy_stream_info( &yuvStreamInfo, &(in->yuvStreamInfo));
}

void Libyuv::dumpInfo (void)
{
    
    std::cerr << "w: " << getWidth() << " h: " << getHeight() <<
    " chroma: " << y4m_si_get_chroma(&yuvStreamInfo) <<
    " interlace: " << y4m_si_get_interlace(&yuvStreamInfo) <<
    "\n";
    
}
