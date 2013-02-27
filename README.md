libav2yuv
=========

Full featured video and audio decompressor.  Allows EDL based editing, with transitions and video generators.

    usage: libav2yuv [-stream<stream>] [-interlace p|b|t] [-frameRate <rate>] [-aspect <aspect>] [-chroma <chroma>] [-out <outputfile>] <filename>
    converts any media file recognised by libav to yuv4mpeg stream

    -w|--audio              Write a PCM file not a video file
    -I|--interlace <pbt>    Force interlace mode overides parameters read from media file
    -F|--frameRate <n:d>    Force framerate. This is not
    -f|--format <fmt>       Force format type (if incorrectly detected)
    -A|--aspect (<n:d>|PAL|PAL_WIDE|NTSC|NTSC_WIDE) Force aspect ratio
    -S|--chroma <chroma>    Force chroma subsampling mode. Only use this if you know what you are doing, or using -convert
                            if the mode in the stream is unsupported will upsample to YUV444
    -c|--convert            Force conversion to chroma mode (requires a chroma mode)
    -s|--stream             select stream other than stream 0
    -o|--out <outputfile>   write to file rather than stdout
    -r|--range [[[HH:]MM:]SS:]FF-[[[HH:]MM:]SS:]FF playout only these frames
    -E|--extension          enable y4m extensions (may be required if source file is not a common format)
    -h|--help               print this help
