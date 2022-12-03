#include "../sylar/sylar.h"
#include "../sylar/bytearray.h"
#include <cstdlib>
#include <ostream>
#include <vector>
#include "../sylar/macro.h"


void test(){
#define XX(type,len,write_fun,read_fun,base_len) {\
	std::vector<type> vec; \
	sylar::ByteArray::ptr ba(new sylar::ByteArray(base_len)); \
	for(int i = 0; i < len; i++){ \
		vec.push_back(i); \
	} \
	for(auto& i : vec){ \
		ba->write_fun(i); \
	} \
	ba->setPosition(0); \
	for(size_t i = 0 ;i < vec.size();i++){ \
		type v = ba->read_fun();\
		SYLAR_ASSERT(v == vec[i]); \
	} \
	SYLAR_ASSERT(ba->getReadSize() == 0);\
	SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #write_fun <<  "/"<<#read_fun <<#type <<" len: " <<len; \
	ba->setPosition(0);\
	std::string name = "./" #type "_" #len ".dat"; \
	SYLAR_ASSERT(ba->writeToFile(name));\
	sylar::ByteArray::ptr ba1(new sylar::ByteArray(1));\
	ba1->readFromFile(name); \
	ba1->setPosition(0); \
	for(size_t i = 0 ;i < vec.size();i++){ \
		type v = ba1->read_fun();\
		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "v--"<<v<<"--vec[i]--"<<vec[i];\
		SYLAR_ASSERT(v == vec[i]); \
	} \
	ba1->setPosition(0); \
	SYLAR_ASSERT(ba->toString() == ba1->toString());\
	SYLAR_ASSERT(ba->getPosition() == 0); \
}
	XX(int8_t,100,writeFint8,readFint8,1);
	XX(int64_t,100,writeFint64,readFint64,1);
	XX(uint64_t,100,writeFuint64,readFuint64,1);
	XX(uint8_t,100,writeFuint8,readFuint8,1);
	XX(int16_t,2,writeFint16,readFint16,1);
	XX(uint16_t,2,writeFuint16,readFuint16,1);
	XX(int32_t,100,writeFint32,readFint32,6);
	XX(uint32_t,100,writeFuint32,readFuint32,5);


	XX(int32_t,2,writeInt32,readInt32,1);
	XX(uint32_t,10,writeUint32,readUint32,6);

	XX(int64_t,100,writeInt64,readInt64,7);
	XX(uint64_t,100,writeUint64,readUint64,6);


#undef XX

}

void test_string(){
	sylar::ByteArray::ptr ba(new sylar::ByteArray(1));
	std::string str = "aaaaaaaabbbbbbbbbbbb";
	std::string name = "./file";
	ba->writeStringF16(str);
	ba->setPosition(0);
	SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << ba->toString();
	ba->writeToFile(name);

}

int main(){
	sylar::IOManager iom(2);
	iom.scheduler(test_string);
	return 0;

}
