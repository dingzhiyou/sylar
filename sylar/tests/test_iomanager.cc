#include "../sylar/sylar.h"
#include <endian.h>
#include  <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include<arpa/inet.h>



static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("root");

void test_fiber(){
	SYLAR_LOG_INFO(g_logger) << "test_fiber";
}

void test1(){
	sylar::IOManager iom(4);
	int sock = ::socket(AF_INET,SOCK_STREAM,0);
	SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "socket " <<sock;
	//fcntl(sock, F_SETFL,O_NONBLOCK);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	inet_pton(AF_INET,"183.232.231.172",&addr.sin_addr.s_addr); 
	int ret = connect(sock,(sockaddr*)&addr, sizeof(addr));
	SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "ret: " <<ret;
	//iom.addEvent(sock, sylar::IOManager::WRITE, &test_fiber);
	//iom.addEvent(sock, sylar::IOManager::READ, &test_fiber);
	//SYLAR_LOG_INFO(g_logger) <<"EventCount : " <<  iom.getEventCount();
	char buf[4096] = {0};
	std::string ss("GET / HTTP/1.1");
	size_t rt = send(sock,ss.c_str(),ss.size() ,0);
	SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "rt: " <<rt;
	size_t s = recv(sock,buf ,sizeof(buf),0);
	
	SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "s: " <<s;
	SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "1111";
	std::string  str(buf,s);
	SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "2222";
	SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << str;
	
}

sylar::Timer::ptr s_timer;
void test_tiemr(){
	sylar::IOManager iom(2);
	 s_timer = iom.addTimer(1000, [](){
			static int i = 5;
			SYLAR_LOG_INFO(g_logger) << "test_fiber";

			if(--i == 0){
				s_timer->cancle();
			}
		},true);

}

int main(int argc,char** argv){
	test1();
//	test_tiemr();
	return 0;
}


