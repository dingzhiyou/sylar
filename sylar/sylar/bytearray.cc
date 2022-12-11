#include "bytearray.h"
#include <bits/types/struct_iovec.h>
#include <iostream>
#include <iomanip>
#include"log.h"
#include<cmath>
#include "endian.h"
#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>
#include <cstring>
#include <fstream>
#include <ios>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
namespace sylar{

ByteArray::Node::Node(size_t s):ptr(new char[s])
       	    ,size(s)
	    ,next(nullptr){}
ByteArray::Node::Node():ptr(nullptr)
			,size(0)
			,next(nullptr){
	
}
ByteArray::Node::~Node(){
	if(ptr){
		delete[] ptr;
	}
}
		
ByteArray::ByteArray(size_t base_size):m_baseSize(base_size)
				      ,m_position(0)
				      ,m_capacity(base_size)
				      ,m_size(0)
				      ,m_endian(SYLAR_BIG_ENDIAN)
				      ,m_root(new Node(base_size))				         ,m_cur(m_root){
	
}
bool ByteArray::isLittleEndian()const {
	return m_endian == SYLAR_LITTLE_ENDIAN;
}
void ByteArray::setIsLittleEndian(bool v){
	if(v){
		m_endian = SYLAR_LITTLE_ENDIAN;
	}else{
		m_endian = SYLAR_BIG_ENDIAN;
	}
}

ByteArray::~ByteArray(){
	Node* tmp = m_root;
	if(tmp){
		m_cur = tmp;
		tmp = tmp->next;
		delete m_cur;
	}
}
void ByteArray::writeFint8(int8_t value){
	write(&value, sizeof(value));
}
void ByteArray::writeFuint8(uint8_t value){
	write(&value, sizeof(value));
}
void ByteArray::writeFint16(int16_t value){
	if(SYLAR_BYTE_ORDER == m_endian){
		write(&value, sizeof(value));
	}else{
		value = byteswap(value);
		write(&value, sizeof(value));
	}
}
void ByteArray::writeFuint16(uint16_t value){	
	if(SYLAR_BYTE_ORDER == m_endian){
		write(&value, sizeof(value));
	}else{
		value = byteswap(value);
		write(&value, sizeof(value));
	}
}
void ByteArray::writeFint32(int32_t value){
	if(SYLAR_BYTE_ORDER == m_endian){
		write(&value, sizeof(value));
	}else{
		value = byteswap(value);
		write(&value, sizeof(value));
	}
}
void ByteArray::writeFuint32(uint32_t value){
	if(SYLAR_BYTE_ORDER == m_endian){
		write(&value, sizeof(value));
	}else{
		value = byteswap(value);
		write(&value, sizeof(value));
	}
}
void ByteArray::writeFint64(int64_t value){
	if(SYLAR_BYTE_ORDER == m_endian){
		write(&value, sizeof(value));
	}else{
		value = byteswap(value);
		write(&value, sizeof(value));
	}
}
void ByteArray::writeFuint64(uint64_t value){
	if(SYLAR_BYTE_ORDER == m_endian){
		write(&value, sizeof(value));
	}else{
		value = byteswap(value);
		write(&value, sizeof(value));
	}
}



static uint32_t EncodeZigzag32(const int32_t& v){
	if(v < 0){
		return ((uint32_t)(-v)) * 2 -1;
	}else{
		return v * 2;
	}
}

static uint64_t EncodeZigzag64(const int64_t& v){
	if(v < 0){
		return ((uint64_t)(-v)) * 2 -1;
	}else{
		return v * 2;
	}
}

static uint32_t DecodeZigzag32(const uint32_t& v){
	return (v >> 1) ^ -(v & 1);
}

static uint64_t DecodeZigzag64(const uint64_t& v){
	return (v >> 1) ^ -(v & 1);
}
void ByteArray::writeInt32(int32_t value){
	writeUint32(EncodeZigzag32(value));
}
void ByteArray::writeUint32(uint32_t value){
	uint8_t tmp[5];
	uint8_t i = 0;
	while(value >= 0x80){
		tmp[i++] = (value & 0x7F) | 0x80;
		value >>= 7;
	}

	tmp[i++] = value;
	write(tmp,i);

}
void ByteArray::writeInt64(int64_t value){
	writeUint64(EncodeZigzag64(value));
}
void ByteArray::writeUint64(uint64_t value){
	uint8_t tmp[10];
	uint8_t i = 0;
	while(value >= 0x80){
		tmp[i++] = (value & 0x7F) | 0x80;
		value >>= 7;
	}

	tmp[i++] = value;
	write(tmp,i);
}


void ByteArray::writeFloat(float value){
	uint32_t v;
	memcpy(&v,&value,sizeof(value));
	writeFuint32(v);
}
void ByteArray::writeDouble(double value){
	uint64_t v;
	memcpy(&v,&value,sizeof(value));
	writeFuint64(v);

}
void ByteArray::writeStringF16(std::string& value){
	writeFuint16((uint16_t)value.size());
	write(value.c_str(),value.size());

}
void ByteArray::writeStringF32(std::string& value){
	writeFuint32((uint32_t)value.size());
	write(value.c_str(),value.size());
}
void ByteArray::writeStringF64(std::string& value){
	writeFuint64((uint64_t)value.size());
	write(value.c_str(),value.size());
}
void ByteArray::writeStringVint(std::string& value){
	writeUint64((uint64_t)value.size());
	write(value.c_str(),value.size());
}
void ByteArray::writeStringWithoutLength(std::string& value){
	write(value.c_str(),value.size());
}


int8_t   ByteArray::readFint8(){
	int8_t v;
	read(&v, sizeof(v));
	return v;
}
uint8_t  ByteArray::readFuint8(){
	uint8_t v;
	read(&v, sizeof(v));
	return v;
}
int16_t  ByteArray::readFint16(){
	int16_t v;
	read(&v,sizeof(v));

	if(m_endian == SYLAR_BYTE_ORDER){
		SYLAR_LOG_DEBUG(SYLAR_LOG_ROOT()) <<"m_endian---------------------";
		return v;
	}
	SYLAR_LOG_DEBUG(SYLAR_LOG_ROOT()) <<"m_endian---------------------";
	return byteswap(v);
}
uint16_t ByteArray::readFuint16(){
	uint16_t v;
	read(&v,sizeof(v));
	if(m_endian == SYLAR_BYTE_ORDER){
		return v;
	}
	return byteswap(v);

}
int32_t  ByteArray::readFint32(){
	int32_t v;
	read(&v,sizeof(v));
	if(m_endian == SYLAR_BYTE_ORDER){
		return v;
	}
	return byteswap(v);

}
uint32_t ByteArray::readFuint32(){
	uint32_t v;
	read(&v,sizeof(v));
	if(m_endian == SYLAR_BYTE_ORDER){
		return v;
	}
	return byteswap(v);

}

int64_t  ByteArray::readFint64(){
	int64_t v;
	read(&v,sizeof(v));
	if(m_endian == SYLAR_BYTE_ORDER){
		return v;
	}
	return byteswap(v);

}
uint64_t ByteArray::readFuint64(){
	uint64_t v;
	read(&v,sizeof(v));
	if(m_endian == SYLAR_BYTE_ORDER){
		return v;
	}
	return byteswap(v);

}

int32_t  ByteArray::readInt32(){
	return DecodeZigzag32(readUint32());

}
uint32_t ByteArray::readUint32(){
	uint32_t result = 0;
	for(int i = 0;i < 32;i += 7){
		uint8_t b = readFuint8();
		if(b < 0x80){
			result |= ((uint32_t)b) << i;
			break;
		}else{
			result |= (((uint32_t)(b & 0x7f)) << i);
		}
	}
	return result;
}
int64_t  ByteArray::readInt64(){
	return DecodeZigzag64(readUint64());
}
int64_t  ByteArray::readUint64(){
	uint64_t result = 0;
	for(int i = 0;i < 64;i += 7){
		uint8_t b = readFuint8();
		if(b < 0x80){
			result |= ((uint64_t)b) << i;
			break;
		}else{
			result |= (((uint64_t)(b & 0x7F)) << i);
		}
	}
	return result;
}

float   ByteArray::readFloat(){
	uint32_t v = readFuint32();
	float value;
	memcpy(&v,&value , sizeof(v));
	return value;

}
double   ByteArray::readDouble(){
	uint64_t v = readFuint64();
	double value;
	memcpy(&v,&value , sizeof(v));
	return value;
}


std::string ByteArray::readStringF16(){
	uint16_t len = readFuint16();
	std::string buf;
	buf.resize(len);
	read(&buf[0], len);
	return buf;
}
std::string ByteArray::readStringF32(){
	uint32_t len = readFuint32();
	std::string buf;
	buf.resize(len);
	read(&buf[0], len);
	return buf;

}
std::string ByteArray::readStringF64(){
	uint64_t len = readFuint64();
	std::string buf;
	buf.resize(len);
	read(&buf[0], len);
	return buf;

}
std::string ByteArray::readStringVint(){
	uint64_t len = readUint64();
	std::string buf;
	buf.resize(len);
	read(&buf[0], len);
	return buf;
}


void ByteArray::clear(){
	Node* tmp = m_root->next;
	while(tmp){
		SYLAR_LOG_DEBUG(SYLAR_LOG_ROOT()) << "in clear[[[[[[[[[[";
		m_cur = tmp;
		tmp = tmp->next;
		delete m_cur;
	}
	m_size = 0;
	m_capacity = m_baseSize;
	m_cur = m_root;
	m_root->next = nullptr;
}
void ByteArray::write(const void* buf,size_t size){
	if(size <= 0){
		return;
	}
	addCapacity(size);
	size_t npos = m_position % m_baseSize;
	size_t ncap = m_cur->size - npos;
	size_t bpos = 0;
	
	while(size > 0){
		if(ncap >= size){
			memcpy(m_cur->ptr + npos,(char*)buf + bpos,size);
			if(m_cur->size == (size + npos)){
				m_cur = m_cur->next;
			}
			m_position += size;
			bpos += size;
			size = 0;
		}else{
			memcpy(m_cur->ptr + npos,(char*)buf + bpos, ncap);
			m_position += ncap;
			bpos += ncap;
			size -= ncap;
			m_cur = m_cur->next;
			ncap = m_cur->size;
			npos = 0;
		}

		if(m_position > m_size){
			m_size = m_position;
		}
	}
}
void ByteArray::read(void* buf,size_t size){
	if(size > getReadSize()){
		throw std::out_of_range("read size too long");
	}

	size_t npos = m_position % m_baseSize;
	size_t ncap = m_cur->size - npos;
	size_t bpos = 0;

	while(size > 0){
		if(ncap >= size){
			memcpy((char*)buf + bpos,m_cur->ptr + npos, size);
			if(m_cur->size == npos + size){
				m_cur = m_cur->next;
			}

			m_position += size;
			bpos += size;
			size = 0;
		}else{
			memcpy((char*)buf + bpos,m_cur->ptr + npos,ncap);
			m_position += ncap;
			bpos += ncap;
			size -= ncap;
			m_cur = m_cur->next;
			ncap = m_cur->size;
			npos = 0;
		}
	}

}

void ByteArray::read(void* buf,size_t size,size_t position){
	if(size > getReadSize()){
		throw std::out_of_range("read size too long");
	}

	size_t npos = position % m_baseSize;
	size_t ncap = m_cur->size - npos;
	size_t bpos = 0;
	Node* cur = m_cur;

	while(size > 0){
		if(ncap >= size){
			memcpy((char*)buf + bpos,cur->ptr + npos, size);
			if(cur->size == npos + size){
				cur = cur->next;
			}

			position += size;
			bpos += size;
			size = 0;
		}else{
			memcpy((char*)buf + bpos,cur->ptr + npos,ncap);
			position += ncap;
			bpos += ncap;
			size -= ncap;
			cur = cur->next;
			ncap = cur->size;
			npos = 0;
		}
	}



}
std::string ByteArray::toString(){
	std::string str;
	str.resize(getReadSize());
	if(str.empty()){
		return str;
	}
	read(&str[0],str.size(),m_position);
	return str;
}
std::string ByteArray::toHexString(){
	std::stringstream ss;
	std::string str = toString();

	for(size_t i = 0; i < str.size();i++){
		if(i > 0 && i % 32 == 0){
			ss << std::endl;
		}
		ss<<std::setw(2)<<std::setfill('0')<<std::hex
		  <<(int)(uint8_t)str[i] << " ";
	}
	return ss.str();

}
void ByteArray::setPosition(size_t v){
	if(v > m_capacity){
		throw std::out_of_range("set_position out of range");
	}
	m_position = v;
	if(m_position > m_size){
		m_size = m_position;
	}
	m_cur = m_root;
	while(v >= m_cur->size){
		v-=m_cur->size;
		m_cur = m_cur->next;
	}
	if(v == m_cur->size){
		m_cur = m_cur->next;
	}
}

bool ByteArray::writeToFile(std::string& name)const{
	std::ofstream ofs;
	ofs.open(name,std::ios::trunc | std::ios::binary);
	if(!ofs){
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ofstream error";
		return false;
	}

	int64_t read_size =getReadSize();
	int64_t pos = m_position;
	Node* cur = m_cur;

	while(read_size > 0){
		int diff = pos % m_baseSize;
		int64_t len = (read_size > (int64_t)m_baseSize ? m_baseSize : read_size) - diff;
		ofs.write(cur->ptr + diff, len);
		cur = cur->next;
		pos += len;
		read_size -= len;
	}
	return true;

}
void ByteArray::readFromFile(std::string& name){
	std::ifstream ifs;
	ifs.open(name,std::ios::binary);
	if(!ifs){
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << " ifs error";
		return;
	}
	std::shared_ptr<char> buff(new char[m_baseSize],[](char* ptr){delete[] ptr;});
	while(!ifs.eof()){
		ifs.read(buff.get(), m_baseSize);
		write(buff.get(),ifs.gcount());
	}
}
void ByteArray::addCapacity(size_t size){
	if(size <= 0){
		return ;
	}
	size_t old_cap = getCapacity();
	if(old_cap > size){
		return;
	}

	size = size - old_cap;
	//size_t count = (size / m_baseSize) + (((size % m_baseSize) > old_cap) ? 1 : 0);
	size_t count = ceil(1.0 * size / m_baseSize);
	Node* tmp = m_root;
	while(tmp->next){
		tmp = tmp->next;
	}

	Node* first =  NULL;
	for(size_t i = 0;i < count; i++){
		tmp->next = new Node(m_baseSize);
		if(first == NULL){
			first = tmp->next;
		}
		tmp = tmp->next;
		m_capacity += m_baseSize;
	}
	if(old_cap == 0){
		m_cur = first;
	}
}


uint64_t ByteArray::getReadBuffers(std::vector<iovec>& buffers,uint64_t len){
	len  = getReadSize() > len ? len : getReadSize();
	if(len == 0){
		return 0;
	}
	size_t size = len;
	size_t npos = m_position % m_baseSize;
	size_t ncap = m_cur->size - npos;
	struct iovec vec;
	Node* cur = m_cur;

	while(len){
		if(ncap >= len){
			vec.iov_base = cur->ptr + npos;
			vec.iov_len = len;
			len = 0;
		}else{
			vec.iov_base = cur->ptr + npos;
			vec.iov_len = ncap;
			len -= ncap;
			cur = cur->next;
			ncap = cur->size;
			npos = 0;
		}
		buffers.push_back(vec);
	}
	return size;
}
uint64_t ByteArray::getReadBuffers(std::vector<iovec>& buffers,uint64_t len,uint64_t position){
	len  = (m_size - position) > len ? len : (m_size - position);
	if(len == 0){
		return 0;
	}
	size_t size = len;
	size_t npos = position % m_baseSize;
	size_t count = position / m_baseSize;
	Node* cur = m_root;
        while(count){
       		cur = cur->next;
		count--;
        }	
	size_t ncap = m_cur->size - npos;
	struct iovec vec;
	while(len){
		if(ncap >= len){
			vec.iov_base = cur->ptr + npos;
			vec.iov_len = len;
			len = 0;
		}else{
			vec.iov_base = cur->ptr + npos;
			vec.iov_len = ncap;
			len -= ncap;
			cur = cur->next;
			ncap = cur->size;
			npos = 0;
		}
		buffers.push_back(vec);
	}
	return size;

}
uint64_t ByteArray::getWriteBuffers(std::vector<iovec>& buffers,uint64_t len){
	if(len == 0){
		return 0;
	}
	addCapacity(len);
	size_t size = len;
	size_t npos = m_position % m_baseSize;
	size_t ncap = m_cur->size - npos;
	struct iovec vec;
	Node* cur = m_cur;

	while(len > 0){
		if(ncap >= len){
			vec.iov_base = cur->ptr + npos;
			vec.iov_len = len;
			len = 0;
		}else{
			vec.iov_base = cur->ptr + npos;
			vec.iov_len = ncap;
			len -= ncap;
			cur = cur->next;
			ncap = cur->size;
			npos = 0;
		}
		buffers.push_back(vec);
	}
	return size;
}

}
