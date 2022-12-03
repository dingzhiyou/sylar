#ifndef __SYALR_BYTEARRAY_H__
#define __SYALR_BYTEARRAY_H__

#include<memory>
#include<sys/socket.h>
#include <sys/types.h>
#include<vector>
#include <string>
#include<string.h>
namespace sylar{
	
class ByteArray{
public:
	typedef std::shared_ptr<ByteArray> ptr;
	struct Node{
		Node(size_t s);
		Node();
		~Node();
		char* ptr;
		size_t size;
		Node* next;
	};
	
	ByteArray(size_t base_size = 4096);
	~ByteArray();

	void read(void* buf,size_t size,size_t position);
	std::string toString();
	std::string toHexString();
	//write
	void writeFint8(int8_t value);
	void writeFuint8(uint8_t value);
	void writeFint16(int16_t value);
	void writeFuint16(uint16_t value);
	void writeFint32(int32_t value);
	void writeFuint32(uint32_t value);
	void writeFint64(int64_t value);
	void writeFuint64(uint64_t value);

	void writeInt32(int32_t value);
	void writeUint32(uint32_t value);
	void writeInt64(int64_t value);
	void writeUint64(uint64_t value);


	void writeFloat(float value);
	void writeDouble(double value);
	void writeStringF16(std::string& value);
	void writeStringF32(std::string& value);
	void writeStringF64(std::string& value);
	void writeStringVint(std::string& value);
	void writeStringWithoutLength(std::string& value);


	int8_t   readFint8();
	uint8_t  readFuint8();
	int16_t  readFint16();
	uint16_t readFuint16();
	int32_t  readFint32();
	uint32_t readFuint32();
	int64_t  readFint64();
	uint64_t readFuint64();

	int32_t  readInt32();
	uint32_t readUint32();
	int64_t  readInt64();
	int64_t  readUint64();

	float    readFloat();
	double   readDouble();


	std::string readStringF16();
	std::string readStringF32();
	std::string readStringF64();
	std::string readStringVint();


	void clear();
	void write(const void* buf,size_t size);
	void read(void* buf,size_t size);

	size_t getPosition()const {return m_position;};
	void setPosition(size_t v);

	bool writeToFile(std::string& name)const;
	void readFromFile(std::string& name);


	size_t getBaseSize()const {return m_baseSize;}
	size_t getReadSize()const {return m_size - m_position;}

	bool isLittleEndian()const;
	void setIsLittleEndian(bool v);

	uint64_t& getSize(){return m_size;}
	uint64_t getReadBuffers(std::vector<iovec>& buffers,uint64_t len = ~0ull);
	uint64_t getReadBuffers(std::vector<iovec>& buffers,uint64_t len,uint64_t position);
	uint64_t getWriteBuffers(std::vector<iovec>& buffers,uint64_t len);
private:

	void addCapacity(size_t size);
	size_t getCapacity()const {return m_capacity - m_position;}

private:
	size_t m_baseSize;//一个Node初始多大
	size_t m_position;//当前操作的位置
	size_t m_capacity;//当前的容量
	size_t m_size;//当前的大小
	int8_t m_endian;
	Node* m_root;
	Node* m_cur;




};


}




#endif
