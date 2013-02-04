#include "AVException.h"

AVException::AVException(std::string m, enum AVExceptionCode c)
{
	code = c;
	message = std::string(m);
}

AVException::~AVException() throw ()
{
	std::cerr << "Exiting Exception\n";	
}

std::string AVException::getMessage(void) { return message; }
enum AVExceptionCode AVException::getCode(void) { return code; }
