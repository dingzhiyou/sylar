#include "../sylar/tcp_server.h"
#include "../sylar/sylar.h"
#include <vector>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

void run(){
	auto addr = sylar::IPAddress::LookupAny("0.0.0.0:8033");
	//auto addr2 = sylar::UnixAddress::ptr(new sylar::UnixAddress("/tmp/unix_addr"));
	//SYLAR_LOG_INFO(g_logger)<< addr->toString()<< "---"<<addr2->toString();
	std::vector<sylar::Address::ptr> addrs;
	std::vector<sylar::Address::ptr> fieldaddrs;

	addrs.push_back(addr);
	//addrs.push_back(addr2);
	sylar::TcpServer::ptr tcp_server(new sylar::TcpServer);
	tcp_server->bind(addrs,fieldaddrs);
	tcp_server->start();
}
int main(){
	sylar::IOManager iom(2);
	iom.scheduler(run);
	return 0;
}
