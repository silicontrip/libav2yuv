# pragma once
#import <string>
#import <iostream>

enum AVExceptionCode {

	MEMORY_ALLOCATION_ERROR,
	INVALID_ARGUMENT,
	UNSUPPORTED_CHROMA,
	IO_ERROR,
	FILE_ERROR
};

class AVException: public std::exception {

	std::string message;
	enum AVExceptionCode code;
public:
	AVException(std::string,enum AVExceptionCode);
	~AVException() throw();
	std::string getMessage(void);
	enum AVExceptionCode getCode(void);
};