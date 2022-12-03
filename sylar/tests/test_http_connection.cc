#include "../http/http_connection.h"
#include "../sylar/sylar.h"
#include <map>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
void run(){
	//auto addr = sylar::IPAddress::LookupAnyIPAddress("www.baidu.com:80");
	//SYLAR_LOG_INFO(g_logger)<<"port: "<<addr->getPort();
	////auto addr2 = sylar::IPAddress::LookupAny("www.baidu.com");
	//if(!addr){
	//	SYLAR_LOG_ERROR(g_logger) << "addr error";
	//}
	//sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
	//if(!sock){
	//	SYLAR_LOG_ERROR(g_logger) << "sock error";
	//}
	//sock->connect(addr);
	//sylar::http::HttpConnection::ptr conn(new sylar::http::HttpConnection(sock));
	//sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);

	//SYLAR_LOG_INFO(g_logger) << req->toString();
	//int rt = conn->sendRequest(req);
	//SYLAR_LOG_INFO(g_logger) <<"rt : "<<rt ;
	//auto rsp = conn->rcvResponse();
	//if(!rsp){
	//	SYLAR_LOG_INFO(g_logger) <<"rsp is nullptr " ;
	//}
	//SYLAR_LOG_INFO(g_logger) << rsp->toString();



	SYLAR_LOG_INFO(g_logger) << "=====================================";

	SYLAR_LOG_INFO(g_logger) << "=====================================";
	auto rsp2 = sylar::http::HttpConnection::DoGet("https://m.weibo.cn",2);
	SYLAR_LOG_DEBUG(g_logger)<< rsp2->result <<" --- "<<rsp2->error<<"---"<<rsp2->response->toString();
}

void test_pool(){
	sylar::http::HttpConnectionPool::ptr pool(new sylar::http::HttpConnectionPool("www.baidu.com","",80,4000,1000 ,20));
	
	//std::map<std::string, std::string> headers;
	//headers["Connection"] = "Keep-alive";
	sylar::IOManager::GetThis()->addTimer(1,[pool](){
			auto result = pool->doGet("/",1000);
			SYLAR_LOG_DEBUG(g_logger) << result->toString();
			} ,true);
}


int main(){
	sylar::IOManager iom(2);
	iom.scheduler(test_pool);
	return 0;


}
