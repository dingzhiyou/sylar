#include "../sylar/sylar.h"
#include "../sylar/log.h"
#include "../sylar/ipaddr.h"
#include <map>
#include <vector>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
void test(){
	std::vector<sylar::Address::ptr> addrs;
	bool v = sylar::Address::Lookup(addrs,"www.github.com");
	if(!v){
		SYLAR_LOG_ERROR(g_logger) << "find www.baidu.com err";
	}

	for(auto it : addrs){
		SYLAR_LOG_INFO(g_logger) << it->toString();
	}
}

void test_iface(){
	std::multimap<std::string,std::pair<sylar::Address::ptr,uint32_t> > result;
	bool v = sylar::Address::GetInterfaceAddress(result);
	if(!v){
		SYLAR_LOG_ERROR(g_logger) << "find network err";
	}

	for(auto& i : result){
		SYLAR_LOG_INFO(g_logger) << i.first <<" - "<<i.second.first->toString()<<" - "<<i.second.second ;
	}

}
void test_ipv4(){
	auto addr = sylar::IPAddress::Create("127.0.0.1");
	if(addr){
		SYLAR_LOG_INFO(g_logger) << addr->toString();
	}
	
}

int main(){
	sylar::IOManager iom(2);
	iom.scheduler(test_ipv4);
	return 0;

}
