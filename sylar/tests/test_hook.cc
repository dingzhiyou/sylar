#include "../sylar/sylar.h"


sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
void test_hook(){
	sylar::IOManager iom(1);
	iom.scheduler([](){
			sleep(2);
			SYLAR_LOG_INFO(g_logger) << "sleep 2";
		});
	iom.scheduler([](){
			sleep(3);
			SYLAR_LOG_INFO(g_logger) << "sleep 3";
		});



}

int main(){

	test_hook();

}

