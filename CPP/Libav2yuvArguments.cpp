#include "Libav2yuvArguments.h"

Libav2yuvArguments::Libav2yuvArguments(int argc, char** argv)
{
	
	const static struct option legal_flags[] = 
	{
		{"audio", no_argument, 0, 'w'},
		{"interlace", required_argument, 0, 'I'},
		{"frameRate", required_argument, 0, 'F'},
		{"format", required_argument, 0, 'f'},
		{"aspect", required_argument, 0, 'A'},
		{"chroma", required_argument, 0, 'S'},
		{"convert", 	no_argument, 0, 'c'},
		{"stream" , required_argument, 0, 's'},
		{"out", required_argument, 0, 'o'},
		{"range", required_argument, 0, 'r'},
		{"extension", no_argument, 0, 'E'},
		{"help", no_argument,0, 'h'},
		{ 0, 0, 0, 0 }
		
	};
	
	
	
	int c=0;
	int option_index = 0;
	
	// std::cerr<<"argc: " << argc <<"\n";
	
	audio = false;
	extensions = false;
	convert = false;
	interlace = -1;
	frameRate.d = 0;
	aspect.d = 0;
	chroma = -1;
	stream = -1;
	
	
	while (c != -1) {
		c = getopt_long (argc, argv, "wI:f:F:A:s:cS:o:r:Eh",
						 legal_flags, &option_index);
		
		switch(c) {
			case 'a':
				audio=true;
				break;
			case 'E':
				extensions =true;
				break;
			case 'c':
				convert=true;
				break;
			case 'i':
				switch (optarg[0]) {
					case 'P':
					case 'p':  interlace = Y4M_ILACE_NONE;  break;
					case 'T':
					case 't':  interlace = Y4M_ILACE_TOP_FIRST;  break;
					case 'B':
					case 'b':  interlace = Y4M_ILACE_BOTTOM_FIRST;  break;
					default:
						// argument error
						break;
				}
				break;
			case 'F':
				if( Y4M_OK != y4m_parse_ratio(&frameRate, optarg))
				{
					// argument error
				}
				break;
			case 'A':
				if( Y4M_OK != y4m_parse_ratio(&aspect, optarg))
				{
					if (!strcmp(optarg,PAL)) {
						y4m_parse_ratio(&aspect, "128:117");
					} else if (!strcmp(optarg,PAL_WIDE)) {
						y4m_parse_ratio(&aspect, "512:351");
					} else if (!strcmp(optarg,NTSC)) {
						y4m_parse_ratio(&aspect, "4320:4739");
					} else if (!strcmp(optarg,NTSC_WIDE)) {
						y4m_parse_ratio(&aspect, "5760:4739");
					} 
					}
				break;
			case 'S':
				chroma = y4m_chroma_parse_keyword(optarg);
				if (chroma == Y4M_UNKNOWN) {
					std::cerr<< "Unknown Chroma Mode: " << optind << "\n";
					this->chromaModes();
				}
				break;
			case 'f':
				// copy
				format = std::string(optarg);
				break;
			case 'o':
				outFile = std::string(optarg);
				break;
			case 'r':
				range = std::string(optarg);
				break;
			case 's':
				stream  = atoi(optarg);
				break;
			case 'h':
				this->usage();
				exit(0);
				break;
			case -1:
				break;
			default:
				std::cerr<< "Unknown option: " << c << "\n";
				this->usage();
				break;
				
		}
	}
	
	
	// std::cerr<<"optind: " << optind <<"\n";
	
	optind--;
	argc -= optind;
	argv += optind;
	
	for (c=1; c<argc;c++) {
		std::cerr << "argument: " << argv[c] << "\n";
		arguments.push_back(std::string(argv[c]));
	}
	
	
}

void Libav2yuvArguments::usage(void)
{
	std::cerr <<
	"usage: libav2yuv [-stream<stream>] [-interlace p|b|t] [-frameRate <rate>] [-aspect <aspect>] [-chroma <chroma>] [-out <outputfile>] <filename>\n" <<
	"converts any media file recognised by libav to yuv4mpeg stream\n" << 
	"\n" <<
	"-w|--audio\t\tWrite a PCM file not a video file\n" <<
	"-I|--interlace <pbt>\tForce interlace mode overides parameters read from media file\n" <<
	"-F|--frameRate <n:d>\tForce framerate. This is not\n" <<
	"-f|--format <fmt>\tForce format type (if incorrectly detected)\n" <<
	"-A|--aspect (<n:d>|PAL|PAL_WIDE|NTSC|NTSC_WIDE) Force aspect ratio\n" <<
	"-s|--chroma <chroma>\tForce chroma subsampling mode. Only use this if you know what you are doing, or using -convert\n" <<
	"\t\t\tif the mode in the stream is unsupported will upsample to YUV444\n" <<
	"-c|--convert\t\tForce conversion to chroma mode (requires a chroma mode)\n" <<
	"-s|--stream\t\tselect stream other than stream 0\n" <<
	"-o|--out <outputfile>\twrite to file rather than stdout\n" <<
	"-r|--range [[[HH:]MM:]SS:]FF-[[[HH:]MM:]SS:]FF playout only these frames\n" <<
	"-E|--extension\t\tenable y4m extensions (may be required if source file is not a common format)\n" <<
	"-h|--help\t\tprint this help\n" ;
}	

void Libav2yuvArguments::chromaModes(void)
{
	std::cerr <<
	"VALID Chroma Values:\n" <<
	"\t420jpeg   (default)\n" <<
	"\t420mpeg2\n" <<
	"\t420paldv\n" <<
	"\t411\n" <<
	"\t422\n" <<
	"\t444       - non-subsampled Y'CbCr\n" <<
	"\t444alpha  - Y'CbCr with alpha channel (with Y' black/white point)\n"
	;
}
