#include "../sylar/iomanager.h"
#include"../sylar/sylar.h"
#include "../sylar/log.h"
#include "../sylar/daemon.h"
#include "../sylar/fiber.h"
static sylar::Timer::ptr timer;
int test_daemo(int argc,char** argv){
	sylar::IOManager iom(2);
	timer = iom.addTimer(1000,[](){
		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<sylar::ProcessInfoMgr::GetInstance()->toString(); 
		static int count =0 ;
		if(++count > 10){
			timer->cancle();
		}
	},true);

	return 0;
}


int main(int argc ,char** argv){
	return sylar::start_daemon(argc,argv,test_daemo,true);
}
