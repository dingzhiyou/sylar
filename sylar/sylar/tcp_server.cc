#include "tcp_server.h"
#include "iomanager.h"
#include "ipaddr.h"
#include "socket.h"
#include"log.h"
#include <cstring>
#include <vector>


namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

TcpServer::TcpServer(IOManager* worker,IOManager* acceptWorker)
		    :m_worker(worker)
		    ,m_acceptworker(acceptWorker)
		    ,m_RecvTimeout(1000* 2 * 6)
		    ,m_name("sylar/1.0.0")
		    ,m_isStop(true){
	
}
TcpServer::~TcpServer(){
	for(auto& i : m_socks){
		i->close();
	}
}

bool TcpServer::bind(sylar::Address::ptr addr){
	std::vector<Address::ptr> addrs;
	std::vector<Address::ptr> faildaddr;
	addrs.push_back(addr);
	return bind(addrs,faildaddr);
}
bool TcpServer::bind(std::vector<sylar::Address::ptr>& addrs,std::vector<sylar::Address::ptr>& faildaddr){
	for(auto& i : addrs){
		Socket::ptr sock = Socket::CreateTCP(i);
		if(!sock->bind(i)){
			SYLAR_LOG_ERROR(g_logger) << "bind error"<<i->toString();
			faildaddr.push_back(i);
			continue;
		}
		if(!sock->listen()){
			SYLAR_LOG_ERROR(g_logger) << "listen error"<<i->toString();
			faildaddr.push_back(i);
			continue;
		}
		m_socks.push_back(sock);
	}
	if(!faildaddr.empty()){
		m_socks.clear();
		return false;
	}
	for(auto& i : addrs){
		SYLAR_LOG_DEBUG(g_logger) <<"bind successful: "<< i->toString();
	}
	return true;
}
bool TcpServer::start(){
	if(!m_isStop){
		return true;
	}
	m_isStop = false;
	for(auto& i :m_socks){
	       m_acceptworker->scheduler(std::bind(&TcpServer::startAccept,shared_from_this(),i));
	}
	return true;	
}
bool TcpServer::stop(){
	m_isStop = true;
	auto self = shared_from_this();
	m_acceptworker->scheduler([this,self](){
			for(auto& i : m_socks){
				i->cancleAll();
				i->close();
			}
			m_socks.clear();
	});
	return true;
}


void TcpServer::handleClient(Socket::ptr sock){
	SYLAR_LOG_INFO(g_logger) << "handleClient-----------------------";
}
void TcpServer::startAccept(Socket::ptr sock){
	while(!m_isStop){
		Socket::ptr client = sock->accept();
		if(client){
			SYLAR_LOG_ERROR(g_logger) << "------------------client:"<<client->toString();
			client->setRecvTimerout(m_RecvTimeout);
			m_worker->scheduler(std::bind(&TcpServer::handleClient,shared_from_this(),client));
		}else{
			SYLAR_LOG_ERROR(g_logger) << "accept error"<< "errno: "<<errno<<" errstr: "<<strerror(errno);
		}
	}
}


}
