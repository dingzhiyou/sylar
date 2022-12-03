#include "../sylar/sylar.h"
#include "../sylar/socket.h"
#include "../sylar/ipaddr.h"
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

void test_socket(){

	SYLAR_LOG_INFO(g_logger) << "connected addr:--------------------------- ";
	sylar::IPAddress::ptr addr = sylar::IPAddress::LookupAnyIPAddress("www.baidu.com:80");
	SYLAR_LOG_INFO(g_logger) << "addr----"<< addr->toString();
	sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
	if(!sock->connect(addr)){
		SYLAR_LOG_ERROR(g_logger) << "connect error" << "  addr:" <<addr->toString();
	}else{
		SYLAR_LOG_INFO(g_logger) << "connected addr: "<<addr->toString();
	}
	
	const char buff[] = "GET / HTTP/1.0\r\n\r\n";
	int rt = sock->send(buff,sizeof(buff));
	if(rt <= 0){
		SYLAR_LOG_ERROR(g_logger) << "send err rt = "<<rt ;
		return;
	}

	std::string rcvbuf;
	rcvbuf.resize(4096);
	rt = sock->recv(&rcvbuf[0],4096);
	if(rt < 0){
		SYLAR_LOG_ERROR(g_logger) << "send err rt = "<<rt ;
		return;
	}
	rcvbuf.resize(rt);
	SYLAR_LOG_INFO(g_logger)<<"rcvbuf: "<< rcvbuf;


}

int main(){
	sylar::IOManager iom(2);
	iom.scheduler(test_socket);
	return 0;
}


