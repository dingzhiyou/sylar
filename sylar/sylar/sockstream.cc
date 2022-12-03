#include "sockstream.h"
#include"log.h"
#include <bits/types/struct_iovec.h>
#include <vector>

namespace sylar{

bool SockStream::isConnected(){
	return m_sock->isConnected();
}
SockStream::SockStream(Socket::ptr sock,bool own):m_sock(sock)
						 ,m_own(own){

}
SockStream::~SockStream(){
	if(m_own && m_sock){
		m_sock->close();
	}
}
int SockStream::read(void* buffer,size_t length){
	if(!m_sock->isConnected()){
		return -1;
	}
	int rt = m_sock->recv(buffer,length);
	return rt;
}
int SockStream::read(ByteArray::ptr ba,size_t length){
	if(!m_sock->isConnected()){
		return -1;
	}
	std::vector<iovec> iovs;
	ba->getWriteBuffers(iovs,length);
	int rt = m_sock->recv(&iovs[0],iovs.size());
	if(rt > 0){
		ba->setPosition(ba->getPosition() + rt);
	}
	return rt;
}
int SockStream::write(const void* buffer,size_t length){
	if(!m_sock->isConnected()){
		return -1;
	}
	int rt = m_sock->send(buffer,length);
	return rt;
}
int SockStream::write(ByteArray::ptr ba,size_t length){
	if(!m_sock->isConnected()){
		return -1;
	}
	std::vector<iovec> iovs;
	ba->getReadBuffers(iovs,length);
	int rt = m_sock->send(&iovs[0],iovs.size());
	if(rt > 0){
		ba->setPosition(ba->getPosition() + rt);
	}
	return rt;
}
int SockStream::close(){
	if(m_sock){
		m_sock->close();
	}
	return 0;
	
}




}
