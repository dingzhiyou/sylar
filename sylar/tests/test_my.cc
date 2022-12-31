#include "../sylar/tcp_server.h"
#include <vector>
#include "../sylar/log.h"
#include<iostream>
#include<memory>


sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("root");

class Test :public sylar::TcpServer{
public:
	typedef std::shared_ptr<Test> ptr;
	void handleClient(sylar::Socket::ptr sock){
		vs.push_back(sock);
		SYLAR_LOG_INFO(g_logger) << "vs.size(): "<<vs.size();
		std::cout<<"vs.size: "<<vs.size() <<std::endl;
	}
private:
	std::vector<sylar::Socket::ptr> vs;
};

void run(){
	Test::ptr test(new Test);
	auto addr = sylar::Address::LookupAny("0.0.0.0:8066");
	test->bind(addr);
	test->start();

}
int main(){
	sylar::IOManager iom(2);
	iom.scheduler(run);
	return 0;
	
}
