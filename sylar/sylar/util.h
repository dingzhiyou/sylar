#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include<sys/types.h>
#include<sys/syscall.h>
#include<pthread.h>
#include<stdio.h>
#include<stdint.h>
#include<unistd.h>
#include<string>
#include<vector>
#include<sys/time.h>
namespace sylar{
	pid_t GetThreadId();
	uint32_t GetFriberId();
	void Backtrace(std::vector<std::string>& bt,int size,int skip);
	std::string BacktraceToString(int size,int skip);
	uint64_t GetCurrentUs();
	uint64_t GetCurrentMs();


	std::string Time2Str(time_t ts,const std::string& format = "%Y-%m-%d %H:%M:%S");
}



#endif
