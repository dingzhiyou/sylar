#include "../sylar/sylar.h"
#include <string>
#include <unistd.h>

static sylar::Logger::ptr  g_logger = SYLAR_LOG_NAME("system");

void f(){
	SYLAR_LOG_DEBUG(g_logger) << "in thread";
	SYLAR_LOG_DEBUG(g_logger) <<sylar::Thread::GetThis()->getName()
				  << "id" << sylar::GetThreadId()
				  << " id 2 :" << sylar::Thread::GetThis()->getId();

}
int main(){

	for(int i = 0;  i < 5;i++){
		sylar::Thread thread(f,"thread" + std::to_string(i));
	}
	return 0;

}
