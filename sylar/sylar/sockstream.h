#ifndef __SYLAR_SOCKSTREAM_H__
#define __SYLAR_SOCKSTREAM_H__

#include "stream.h"
#include "socket.h"
#include <memory>


namespace sylar{

class SockStream : public Stream{
public:
	typedef std::shared_ptr<SockStream> ptr;
	SockStream(Socket::ptr sock,bool own = false);
	~SockStream();
	int read(void* buffer,size_t length) override;
	int read(ByteArray::ptr ba,size_t length) override;

	int write(const void* buffer,size_t length) override;
	int write(ByteArray::ptr ba,size_t length) override;
	Socket::ptr getSocket(){return m_sock;}
	bool isConnected();
	int close() override;
private:
	Socket::ptr m_sock;
	bool m_own;
};

}


#endif
