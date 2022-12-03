#include"socket.h"
#include"hook.h"
#include"fd_manager.h"
#include "iomanager.h"
#include "ipaddr.h"
#include"macro.h"
#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <bits/types/struct_iovec.h>
#include <bits/types/struct_timeval.h>
#include<netinet/tcp.h>
#include <cctype>
#include <sstream>
#include <sys/time.h>
#include <cerrno>
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "log.h"

namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

Socket::ptr Socket::CreateTCP(Address::ptr address){
	Socket::ptr addr(new Socket(address->getFamily(), TCP,0));
	return addr;
}
Socket::ptr Socket::CreateUDP(Address::ptr address){
	Socket::ptr addr(new Socket(address->getFamily(),UDP,0));
	return addr;

}
Socket::ptr Socket::CreateTCPSocket(){
	Socket::ptr addr(new Socket(IPv4,TCP,0));
	return addr;

}
Socket::ptr Socket::CreateUDPSocket(){
	Socket::ptr addr(new Socket(IPv4,UDP,0));
	return addr;
}
Socket::ptr Socket::CreateUnixTCPSocket(){
	Socket::ptr addr(new Socket(UNIX,TCP,0));
	return addr;

}
Socket::ptr Socket::CreateUnixUDPSocket(){
	Socket::ptr addr(new Socket(UNIX,UDP,0));
	return addr;
}
Socket::Socket(int family,int type,int protocal):m_sock(-1)
						,m_family(family)
						,m_type(type)
						,m_protocol(protocal)
						,m_isConnected(false)
						{

}
Socket::~Socket(){
	close();
}

uint64_t Socket::getSendTimeout(){
	FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(m_sock);
	if(!ctx){
		return -1;
	}
	return ctx->getTimeout(SO_SNDTIMEO);
}
void Socket::setSendTimerout(uint64_t v){
	struct timeval tv;
	tv.tv_sec = (long)(v/1000);
	tv.tv_usec = (long)(v % 1000 * 1000);
	setOption(SOL_SOCKET,SO_SNDTIMEO,tv);
}


uint64_t Socket::getRecvTimeout(){
	FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(m_sock);
	if(!ctx){
		return -1;
	}
	return ctx->getTimeout(SO_RCVTIMEO);
}
void Socket::setRecvTimerout(uint64_t v){
	struct timeval tv;
	tv.tv_sec = (long)(v/1000);
	tv.tv_usec = (long)(v % 1000 * 1000);
	setOption(SOL_SOCKET,SO_SNDTIMEO,tv);
}


bool Socket::getOption(int level,int option,void* result,size_t* len){
	int rt = getsockopt(m_sock,level,option,result,(socklen_t*)len);
	if(rt){
		SYLAR_LOG_DEBUG(g_logger) << "getOption error sock: "<<
			m_sock << "level: " << level <<" errno: "<<errno<<" errstr: "<<strerror(errno);
		return false;
	}
	return true;
}

bool Socket::setOption(int level,int option ,const void* result,size_t len){
	int rt = setsockopt(m_sock,level ,option,result ,(socklen_t)len);
	if(rt){
		SYLAR_LOG_DEBUG(g_logger) << "setOption error sock: "<<
			m_sock << "level: " << level <<" errno: "<<errno<<" errstr: "<<strerror(errno);
		return false;
	}
	return true;

}
Socket::ptr Socket::accept(){
	Socket::ptr sock(new Socket(m_family,m_type,m_protocol));
	int newsock = ::accept(m_sock,nullptr,nullptr);
	if(newsock == -1){
		SYLAR_LOG_ERROR(g_logger) << "accept error" << " sock: "<<m_sock<<" errno: "<<errno<<" errstr: "<<strerror(errno);
		return nullptr;
	}
	if(sock->init(newsock)){
		return sock;
	}
	return nullptr;
}

Address::ptr Socket::getRemoteAddress(){
	if(m_remoteAddress){
		return m_remoteAddress;
	}

	Address::ptr result;

	switch (m_family) {
		case AF_INET:
			result.reset(new IPv4Address());
			break;
		case AF_INET6:
			result.reset(new IPv6Address());
			break;
		case AF_UNIX:
			result.reset(new UnixAddress());
			break;
		default:
			result.reset(new UnknowAddress(m_family));
			break;
	}

	
	socklen_t len = result->getAddrLen();
	if(getpeername(m_sock,result->getAddr(),&len)){
		SYLAR_LOG_ERROR(g_logger) << "getpeername m_sock: "<<m_sock
		       			  <<"errno: "<<errno <<" strerror:" <<std::strerror(errno);
		return Address::ptr(new UnknowAddress(m_family));
	}

	if(m_family == AF_UNIX){
		UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
		addr->setAddrLen(len);
	}
	m_remoteAddress = result;
	return m_remoteAddress;

}
Address::ptr Socket::getLocalAddress(){
	if(m_localAddress){
		return m_localAddress;
	}

	Address::ptr result;

	switch (m_family) {
		case AF_INET:
			result.reset(new IPv4Address());
			break;
		case AF_INET6:
			result.reset(new IPv6Address());
			break;
		case AF_UNIX:
			result.reset(new UnixAddress());
			break;
		default:
			result.reset(new UnknowAddress(m_family));
			break;
	}

	socklen_t len = result->getAddrLen();
	if(getsockname(m_sock,result->getAddr(),&len)){
		SYLAR_LOG_ERROR(g_logger) << "getsockname m_sock: "<<m_sock
		       			  <<"errno: "<<errno <<" strerror:" <<std::strerror(errno);
		return Address::ptr(new UnknowAddress(m_family));
	}

	if(m_family == AF_UNIX){
		UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
		addr->setAddrLen(len);
	}
	m_localAddress= result;
	return m_localAddress;

}

bool Socket::init(int sock){
	FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(sock);
	if(ctx && ctx->isSocket() && !ctx->isClosed()){
		m_sock = sock;
		m_isConnected = true;
		initSock();
		getLocalAddress();
		getRemoteAddress();
		return true;
	}
	return false;


}
void Socket::initSock(){
	int val = 1;
	setOption(SOL_SOCKET,SO_REUSEADDR,val);
	if(m_type == SOCK_STREAM){
		setOption(IPPROTO_TCP,TCP_NODELAY,val);
	}

}
void Socket::newSock(){
	m_sock = ::socket(m_family,m_type,m_protocol);
	if(SYLAR_LICKLY(m_sock != -1)){
		initSock();
	}else{
		SYLAR_LOG_INFO(g_logger) << "newSocket error";
	}
}
bool Socket::bind(const Address::ptr addr){
	if(!isVaild()){
		newSock();	
	}
	if(addr->getFamily() != m_family){
		SYLAR_LOG_ERROR(g_logger) <<"bind sock family: " <<addr->getFamily() << " m_family: "<< m_family
					  <<" addr: "<<addr->toString();
		return false;
	}

	if(::bind(m_sock,addr->getAddr(),addr->getAddrLen())){
		SYLAR_LOG_ERROR(g_logger) <<"bind err "<<"errno-"<<errno<<"-strerror-"<<strerror(errno) ;
		return false;
	}
	getLocalAddress();
	return true;
}
bool Socket::connect(const Address::ptr addr,uint64_t timeout_ms){
	if(!isVaild()){
		newSock();	
	}
	if(addr->getFamily() != m_family){
		SYLAR_LOG_ERROR(g_logger) <<"connect sock family: " <<addr->getFamily() << " m_family: "<< m_family
					  <<" addr: "<<addr->toString();
		return false;
	}
	if(timeout_ms == (uint64_t)-1){
		if(::connect(m_sock,addr->getAddr(),addr->getAddrLen())){
				SYLAR_LOG_ERROR(g_logger) <<"connect sock family: " <<addr->getFamily() << " m_family: "<< m_family
				  <<" addr: "<<addr->toString() << "errno:" << errno <<" errstr: "<<strerror(errno);
				return false;
		}
	}else{
		if(connect_with_timeout(m_sock,addr->getAddr(),addr->getAddrLen(),timeout_ms)){
				SYLAR_LOG_ERROR(g_logger) <<"connect sock family: " <<addr->getFamily() << " m_family: "<< m_family
					  <<" addr: "<<addr->toString();
				return false;
		}
	}
	m_isConnected = true;
	getLocalAddress();
	getRemoteAddress();
	return true;
}
bool Socket::listen(int backlog){
	if(!isVaild()){
		SYLAR_LOG_ERROR(g_logger) << "listen failed sock: " <<m_sock;
		return false;
	}

	if(::listen(m_sock,backlog)){
		SYLAR_LOG_ERROR(g_logger) << "errno: "<<errno <<" strerror: "<<strerror(errno);
		return false;
	}
	return true;
}
bool Socket::close(){
	if(!isConnected() && m_sock == -1){
		return true;
	}

	m_isConnected = false;
	if(m_sock != -1){
		SYLAR_LOG_ERROR(g_logger) << "closed socket: "<<m_sock;
	 	::close(m_sock);
		m_sock = -1;
	}
	return false;

}
int Socket::send(const void* buffer,size_t length,int flag ){
	if(isConnected()){
		return ::send(m_sock,buffer,length,flag);
	}
	return -1;
}
int Socket::send(const iovec* buffers,size_t length,int flag ){
	if(isConnected()){
		msghdr msg;
		memset(&msg,0,sizeof(msg));
		msg.msg_iov = (iovec*)buffers;
		msg.msg_iovlen = length;
		return ::sendmsg(m_sock, &msg, flag);
	}
	return -1;
}
int Socket::sendTo(const void* buffer,size_t length,const Address::ptr to,int flag){
	if(isConnected()){
		return ::sendto(m_sock,buffer,length,flag,to->getAddr(),to->getAddrLen());
	}
	return -1;
}
int Socket::sendTo(const iovec* buffers,size_t length,const Address::ptr to,int flag){
	if(isConnected()){
		msghdr msg;
		memset(&msg,0,sizeof(msg));
		msg.msg_iov = (iovec*)buffers;
		msg.msg_iovlen = length;
		msg.msg_name = (void*)to->getAddr();
		msg.msg_namelen = to->getAddrLen();
		return ::sendmsg(m_sock, &msg, flag);
	}
	return -1;


}

int Socket::recv(void* buffer,size_t length,int flag){
	if(isConnected()){
		return ::recv(m_sock,buffer,length,flag);
	}
	return -1;
}
int Socket::recv(iovec* buffers,size_t length,int flag){
	if(isConnected()){
		msghdr msg;
		memset(&msg,0,sizeof(msg));
		msg.msg_iov = (iovec*)buffers;
		msg.msg_iovlen = length;
		return ::recvmsg(m_sock, &msg, flag);
	}
	return -1;

}
int Socket::recvFrom(void* buffer,size_t length,Address::ptr from,int flag){
		if(isConnected()){
			socklen_t len= from->getAddrLen();
			return ::recvfrom(m_sock, buffer,length,flag,from->getAddr(),&len);
		
		}
		return -1;
}
int Socket::recvFrom(iovec* buffers,size_t length,Address::ptr from,int flag){
	if(isConnected()){
		msghdr msg;
		memset(&msg,0,sizeof(msg));
		msg.msg_iov = (iovec*)buffers;
		msg.msg_iovlen = length;
		msg.msg_name = (void*)from->getAddr();
		msg.msg_namelen = from->getAddrLen();
		return ::recvmsg(m_sock, &msg, flag);
	}
	return -1;


}
bool Socket::isVaild()const {
	return m_sock != -1;
}
int Socket::getError(){
	int error = 0;
	size_t len = sizeof(error);
	if(!getOption(SOL_SOCKET,SO_ERROR,&errno,&len)){
		return -1;
	}
	return error;
}

std::ostream& Socket::dump(std::ostream& os){
	os << "m_sock: "<<m_sock
	   <<"  m_family: "<<m_family
	   <<"  m_type: "<<m_type
	   <<"  m_protocol: "<<m_protocol
	   <<"  localAddress: "<<m_localAddress->toString();
	if(m_remoteAddress){
		os << "  remoteAddress: "<< m_localAddress->toString(); 
	}
	return os;
}

std::string Socket::toString(){
	std::stringstream ss;
	dump(ss);
	return ss.str();

}
bool Socket::cancleRead(){
	return sylar::IOManager::GetThis()->cancelEvent(m_sock,sylar::IOManager::READ);
}
bool Socket::cancleWrite(){
	return sylar::IOManager::GetThis()->cancelEvent(m_sock,sylar::IOManager::WRITE);
}
bool Socket::cancleAccept(){
	return sylar::IOManager::GetThis()->cancelEvent(m_sock,sylar::IOManager::READ);
}
bool Socket::cancleAll(){
	return sylar::IOManager::GetThis()->cancleAll(m_sock);
	
}


}
