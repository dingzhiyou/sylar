#ifndef __SYLAR_STREAM_H__
#define __SYLAR_STREaM_H__

#include<memory>
#include "bytearray.h"

namespace sylar{
	
class Stream{
public:	
	Stream();
	virtual ~Stream(){}
	virtual int read(void* buffer,size_t length) = 0;
	virtual int read(ByteArray::ptr ba,size_t length) = 0;
	virtual int readFix(void* buffer,size_t length);
	virtual int readFix(ByteArray::ptr ba,size_t length);

	virtual int write(const void* buffer,size_t length) = 0;
	virtual int write(ByteArray::ptr ba,size_t length) = 0;
	virtual int writeFix(const void* buffer,size_t length);
	virtual int writeFix(ByteArray::ptr ba,size_t length);

	virtual int close() = 0;
};


}



#endif
