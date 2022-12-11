#ifndef __SYLAR_DAEMON_H__
#define __SYLAR_DAEMON_H__

#include<functional>
#include<sys/types.h>
#include<unistd.h>
#include "singleton.h"
namespace sylar{

struct ProcessInfo{
	pid_t parent_id;
	pid_t main_id;
	uint64_t parent_start_time = 0;
	uint64_t main_start_time = 0;
	uint32_t restart_count = 0;
	std::string toString()const;

};

typedef Singleton<ProcessInfo> ProcessInfoMgr;

int start_daemon(int argc,char** argv ,std::function<int (int, char**)> main_cb
		,bool is_daemo);

}
#endif
