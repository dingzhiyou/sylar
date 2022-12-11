#include"daemon.h"
#include"fiber.h"
#include <cerrno>
#include <ctime>
#include<errno.h>
#include <sstream>
#include<string.h>
#include <functional>
#include <unistd.h>
#include <sys/wait.h>
#include "log.h"
#include"util.h"

namespace sylar{

std::string ProcessInfo::toString()const{
	std::stringstream ss;
	ss <<"parent_id: "<<parent_id
	   <<" main_id: "<<main_id
	   <<" parent_start_time: "<<Time2Str(parent_start_time)
	   <<" main_start_time: "<<Time2Str(main_start_time);

	return ss.str();
}
	
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static int real_start(int argc,char** argv,std::function<int (int, char**)> main_cb){
	return main_cb(argc,argv);
}

static int real_daemon(int argc,char** argv,std::function<int (int, char**)> main_cb){
	daemon(1,0);
	sylar::Fiber::GetThis();
	ProcessInfoMgr::GetInstance()->parent_id = getpid();
	ProcessInfoMgr::GetInstance()->parent_start_time = time(0);
	while(true){
		pid_t pid = fork();
		if(pid == 0){//child
			sylar::Fiber::GetThis();
			ProcessInfoMgr::GetInstance()->main_id = getpid();
			ProcessInfoMgr::GetInstance()->main_start_time = time(0);

			SYLAR_LOG_INFO(g_logger) << "process start:  "<< getpid();
			return real_start(argc, argv, main_cb);
		}else if(pid < 0){
			SYLAR_LOG_ERROR(g_logger) << " fork err errno : "<< errno << " errstr: "<<strerror(errno);					     	     return -1;
		}else{
			int status = 0;
			waitpid(pid,&status,0);
			if(!status){
				SYLAR_LOG_INFO(g_logger) << "child finished pid= "<<pid <<" status: "<<status;
				break;
			}else{
				SYLAR_LOG_ERROR(g_logger) << "child crash pid= "<<pid<<" status: "<<status;		
			}
		}
		ProcessInfoMgr::GetInstance()->restart_count += 1;
		sleep(2);
	}

	return 0;
}
int start_daemon(int argc,char** argv ,std::function<int (int, char**)> main_cb,bool is_daemo){
	if(!is_daemo){
		return real_start(argc, argv, main_cb);
	}
	return real_daemon(argc, argv,main_cb);

}

}
