#ifndef  __SYLAR_SOCKET_H__
#define  __SYLAR_SOCKET_H__

#include <bits/types/struct_iovec.h>
#include<memory>
#include <ostream>
#include <sys/socket.h>
#include "ipaddr.h"
#include"noncopyable.h"
namespace sylar{

class Socket: public std::enable_shared_from_this<Socket>,Noncopyable{
public:
	typedef std::shared_ptr<Socket> ptr;
	typedef std::weak_ptr<Socket> weak_ptr;

	Socket(int family,int type,int protocal = 0);
	~Socket();
	
	enum {
		TCP = SOCK_STREAM,
		UDP = SOCK_DGRAM,
	};

	enum {
		IPv4 = AF_INET,
		IPv6 = AF_INET6,
		UNIX = AF_UNIX,
	};

	static Socket::ptr CreateTCP(Address::ptr address);
	static Socket::ptr CreateUDP(Address::ptr address);
	static Socket::ptr CreateTCPSocket();
	static Socket::ptr CreateUDPSocket();

	static Socket::ptr CreateUnixTCPSocket();
	static Socket::ptr CreateUnixUDPSocket();


	uint64_t getSendTimeout();
	void setSendTimerout(uint64_t v);

	uint64_t getRecvTimeout();
	void setRecvTimerout(uint64_t v);
	
	bool getOption(int level,int option,void* result,size_t* len);
	template<class T>
	bool getOption(int level,int option,T& result){
		size_t length = sizeof(T);
		return getOption(level,option,(void*)&result,&length);
	}

	bool setOption(int level,int option ,const void* result,size_t len);
	template<class T>
	bool setOption(int level,int option,const T& value){
		return setOption(level,option,(const void*)&value,sizeof(T));
	}
	Socket::ptr accept();
	bool init(int sock);
	bool bind(const Address::ptr addr);
	bool connect(const Address::ptr addr,uint64_t timeout_ms = -1);
	bool listen(int backlog = SOMAXCONN);
	bool close();
	int send(const void* buffer,size_t length,int flag = 0);
	int send(const iovec* buffers,size_t length,int flag = 0);
	int sendTo(const void* buffer,size_t length,const Address::ptr to,int flag = 0);
	int sendTo(const iovec* buffers,size_t length,const Address::ptr to,int flag = 0);

	int recv(void* buffer,size_t length,int flag = 0);
	int recv(iovec* buffers,size_t length,int flag = 0);
	int recvFrom(void* buffer,size_t length,Address::ptr from,int flag = 0);
	int recvFrom(iovec* buffers,size_t length,Address::ptr from,int flag = 0);


	Address::ptr getRemoteAddress();
	Address::ptr getLocalAddress();

	int getFamily() const {return m_family;};
	int getType()const {return m_type;};
	int getProtocol() const {return m_protocol;};

	bool isConnected()const {return m_isConnected;};
	bool isVaild()const;
	int getError();

	std::ostream& dump(std::ostream& os);
	std::string toString();

	int getSocket()const{return m_sock;};

	bool cancleRead();
	bool cancleWrite();
	bool cancleAccept();
	bool cancleAll();

private:
	void initSock();
	void newSock();


private:
	int m_sock;
	int m_family;
	int m_type;
	int m_protocol;
	bool m_isConnected;
	
	Address::ptr m_remoteAddress;
	Address::ptr m_localAddress;

};

}



#endif
