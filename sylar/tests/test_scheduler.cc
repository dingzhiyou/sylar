#include "../sylar/sylar.h"
#include <unistd.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

void test_fiber(){
	SYLAR_LOG_INFO(g_logger)<<"test_fiber";
	static int count = 6;
	if(--count > 0){	
		sylar::Scheduler::GetThis()->scheduler(test_fiber);
		sleep(1);
	}
}
int main(int argc ,char** argv){


	sylar::Scheduler sc(6);
	sc.start();
	sc.scheduler(test_fiber);
	sc.stop();
	return 0;
}
