#include "util.h"
#include "fiber.h"
#include <cstdlib>
#include<dirent.h>
#include <fstream>
#include <string>
#include<sys/types.h>
#include"log.h"
#include <bits/types/struct_timeval.h>
#include <ctime>
#include<execinfo.h>
#include <sstream>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
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



std::string Time2Str(time_t ts,const std::string& format ){
	struct tm tm;
	localtime_r(&ts,&tm);
	char buf[64] = {0};
	std::strftime(buf,sizeof(buf),format.c_str(),&tm);
	return buf;
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


void FSUtil::ListAllFiles(std::vector<std::string>& files,const std::string& path,const std::string& prefix){
	if(access(path.c_str(),0) != 0){
		return;
	}
	DIR* dir = opendir(path.c_str());
	if(dir == nullptr){
		return;
	}

	struct dirent* dp = nullptr;
	while((dp = readdir(dir)) != nullptr){
		if(dp->d_type == DT_DIR){
			if(dp->d_name == std::string(".") || dp->d_name == std::string("..")){
				continue;
			}
			ListAllFiles(files,path + "/" + dp->d_name,prefix);
		}else if(dp->d_type == DT_REG){
			std::string filename(dp->d_name);
			if(prefix.empty()){
				files.push_back(path + "/" + filename);
			}else{
				if(filename.size() < prefix.size()){
					continue;			
				}
				if(filename.substr(filename.size() - prefix.size())== prefix){
					files.push_back(path + "/" + dp->d_name);
				}
			}

		}



	}
	closedir(dir);

}
bool FSUtil::IsRunningPidFile(const std::string &file){
	if(access(file.c_str(), F_OK) != 0){
		return true;
	}
	std::ifstream ifs;
	ifs.open(file);
	std::string line;
	if(!ifs ){
		SYLAR_LOG_DEBUG(g_logger) << " ---------- ";
		return false;
	}
	std::getline(ifs,line);
	SYLAR_LOG_DEBUG(g_logger) << "line"<<line;
	if(line.empty()){
		SYLAR_LOG_DEBUG(g_logger) << " ---------- ";
		return false;
	}
	pid_t pid = atoi(line.c_str());
	if(pid < 1){
		SYLAR_LOG_DEBUG(g_logger) << " ---------- ";
		return false;
	}

	if(!kill(pid,0)){
		SYLAR_LOG_DEBUG(g_logger) << " ---------- ";
		return false;
	}
	return true;
}



}

