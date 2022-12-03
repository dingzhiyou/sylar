#include "stream.h"
#include "log.h"
#include <string>
namespace sylar{

Stream::Stream(){

}
int Stream::readFix(void* buffer,size_t length){
	size_t left = length;
	size_t offset = 0;
	while(left > 0){
		int len = read((char*)buffer + offset,left);
		if(len <= 0){
			return len;
		}
		left -= len;
		offset += len;
	}
	return length;
}
int Stream::readFix(ByteArray::ptr ba,size_t length){
	size_t left = length;
	while(left > 0){
		int len = read(ba,left);
		if(len <= 0){
			return len;
		}
		left -= len;
	}
	return length;

}
int Stream::writeFix(const void* buffer,size_t length){
	size_t left = length;
	size_t offset = 0;
	while(left > 0){
		int len = write((const char*)buffer + offset,left);
		if(len <= 0){
			return len;
		}
		left -= len;
		offset += len;
	}
	return length;

}
int Stream::writeFix(ByteArray::ptr ba,size_t length){
	size_t left = length;
	while(left > 0){
		int len = write(ba,left);
		if(len <= 0){
			return len;
		}
		left -= len;
	}
	return length;


}

}
