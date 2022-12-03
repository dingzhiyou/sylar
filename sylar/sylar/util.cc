#include "util.h"
#include "fiber.h"
#include"log.h"
#include <bits/types/struct_timeval.h>
#include<execinfo.h>
#include <sstream>
#include <sys/select.h>
namespace sylar{
static Logger::ptr g_logger = SYLAR_LOG_NAME("system");

pid_t GetThreadId(){
	return syscall(SYS_gettid);
}
uint32_t GetFriberId(){
	return Fiber::GetFiberId();
	
}
void Backtrace(std::vector<std::string> &bt, int size, int skip){
	void** array = (void**)malloc(sizeof(void*) * size);
	size_t s = ::backtrace(array,size);
	char** strings = ::backtrace_symbols(array, s);
	if(strings == NULL){
		SYLAR_LOG_ERROR(g_logger) << "Backtrace strings null";
	}

	for(size_t i = skip; i < s;i++){
		bt.push_back(strings[i]);
	}
	free(array);
	free(strings);
}

std::string BacktraceToString(int size, int skip){
	std::vector<std::string> vec;
	Backtrace(vec, size, skip);
	std::stringstream ss;
	for(size_t i  = 0; i < vec.size(); i++){
		ss << vec[i] << std::endl;
	}
	return ss.str();

}

uint64_t GetCurrentUs(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000) * 1000;	

}
uint64_t GetCurrentMs(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;	

}
}

