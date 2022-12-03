#include "../sylar/tcp_server.h"
#include"../sylar//socket.h"
#include "../sylar/sylar.h"
#include "../sylar/bytearray.h"

#include <bits/types/struct_iovec.h>
#include <cerrno>
#include<memory>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

class EchoServer : public sylar::TcpServer{

public:
	typedef std::shared_ptr<EchoServer> ptr;
	EchoServer(int type);
	void handleClient(sylar::Socket::ptr sock);
	

private:
	int m_type = 1;
};
EchoServer::EchoServer(int type):m_type(type){

}
void EchoServer::handleClient(sylar::Socket::ptr sock){
	sylar::ByteArray::ptr ba(new sylar::ByteArray);
	while(true){
		ba->clear();
		std::vector<iovec> iovs;
		ba->getWriteBuffers(iovs, 1024);
		int rt = sock->recv(&iovs[0],iovs.size());
		if(rt == 0){
			SYLAR_LOG_INFO(g_logger) << "client close sock: " 
						 << sock->getSocket();
			return;
		}else if(rt < 0){
			SYLAR_LOG_ERROR(g_logger) << "client error"<<" errno: "<<errno <<" strerror: "<<strerror(errno);
			return ;		
		}
		SYLAR_LOG_DEBUG(g_logger) << "rt: "<<rt;
		ba->setPosition(ba->getPosition() + rt);
		ba->setPosition(0);
		if(m_type == 1){
			SYLAR_LOG_INFO(g_logger) << ba->toString();
		}else{
			//SYLAR_LOG_INFO(g_logger) << ba->toHexString();
		}
	}

}

void run(){
	EchoServer::ptr echo(new EchoServer(1));
	sylar::Address::ptr addr = sylar::Address::LookupAnyIPAddress("0.0.0.0:8022");

	while(!echo->bind(addr)){
		sleep(2);
	}

	echo->start();


}
int main(){
	sylar::IOManager iom(2);
	iom.scheduler(run);
	return 0;
}

