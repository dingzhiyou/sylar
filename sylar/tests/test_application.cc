#include "../sylar/application.h"
#include "../sylar/log.h"


static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

int main(int argc,char** argv){
	sylar::Application application;
	if(!application.init(argc,argv)){
		SYLAR_LOG_ERROR(g_logger) << "application init error";
		return -1;
	}
	application.run();

	return 0;

}
