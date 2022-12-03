#ifndef __SYLAR_TCP_SERVER_H__
#define __SYLAR_TCP_SERVER_H__

#include<memory>
#include<functional>
#include <vector>
#include"iomanager.h"
#include "socket.h"
#include"ipaddr.h"

namespace sylar{

class TcpServer : public std::enable_shared_from_this<TcpServer>{
public:
	typedef std::shared_ptr<TcpServer> ptr;

	TcpServer(IOManager* worker = sylar::IOManager::GetThis(),IOManager* acceptWorker = sylar::IOManager::GetThis());
	virtual ~TcpServer();

	virtual bool bind(sylar::Address::ptr addr);
	virtual bool bind(std::vector<sylar::Address::ptr>& addr,std::vector<sylar::Address::ptr>& faildaddr);
	virtual bool start();
	virtual bool stop();

	uint64_t getRecvTimeout()const {return m_RecvTimeout;}
	std::string getName()const {return m_name;}
	void setRecvTimeout(const uint64_t v){m_RecvTimeout = v;}
	void setName(const std::string& name){m_name = name;}

	bool isStop()const{return m_isStop;}
protected:
	virtual void handleClient(Socket::ptr sock);
	virtual void startAccept(Socket::ptr sock);
private:
	std::vector<Socket::ptr> m_socks;
	IOManager* m_worker;
	IOManager* m_acceptworker;
	uint64_t m_RecvTimeout;
	std::string m_name;
	bool m_isStop;


};

}



#endif
