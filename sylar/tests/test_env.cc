#include "../sylar/env.h"


int main(int argc,char** argv){
	sylar::EnvMgr::GetInstance()->addHelp("d", "start with daemo");
	sylar::EnvMgr::GetInstance()->addHelp("s", "start with termainl");
	sylar::EnvMgr::GetInstance()->addHelp("p", "print help");

	if(!sylar::EnvMgr::GetInstance()->init(argc, argv)){
		sylar::EnvMgr::GetInstance()->printHlep();
	}
	
	if(sylar::EnvMgr::GetInstance()->has("p")){
		sylar::EnvMgr::GetInstance()->printHlep();
	}

}
