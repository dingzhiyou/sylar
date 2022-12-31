#include "env.h"
#include "log.h"
#include <cstdio>
#include <cstdlib>
#include<iomanip>
#include<string.h>
#include <unistd.h>
#include <utility>
#include<iostream>
namespace sylar{
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");


void Env::del(const std::string& key){
	MutexType::WriteLock lock(m_mutex);
	m_args.erase(key);
}

bool Env::init(int argc,char** argv){
	char link[1024] = {0};
	char path[1024] = {0};
	sprintf(link,"/proc/%d/exe",getpid());
       	readlink(link,path,sizeof(path));	
	m_exe = path;

	auto pos = m_exe.find_last_of('/');
       	m_cwd = m_exe.substr(0,pos) + "/";	

	m_program = argv[0];
	const char* now_key = nullptr;
	for(int i = 1; i < argc;i++){
		if(argv[i][0] == '-'){
			if(strlen(argv[i]) > 1){
				if(now_key){
					add(now_key,"");
				}
				now_key = argv[i] + 1;

			}else{
				
				SYLAR_LOG_ERROR(g_logger) << "invaid arg index: "<<i
							  <<" val = "<<argv[1];
				return false;
			}		
		}else{
				if(now_key){
					add(now_key,argv[i]);
					now_key = nullptr;
				}else{
					SYLAR_LOG_ERROR(g_logger) << "invaid arg index: "<<i
								  <<" val = "<<argv[1];
					return false;
				}
			}

	}
	if(now_key){
		add(now_key,"");
	}
	return true;
}
void Env::add(const std::string& key,const std::string& val){
	MutexType::WriteLock lock(m_mutex);
	m_args[key] = val;
}
bool Env::has(const std::string& key){
	MutexType::ReadLock lock(m_mutex);
	auto it = m_args.find(key);
	return it != m_args.end();
}


void Env::addHelp(const std::string& key,const std::string& val){
	MutexType::WriteLock lock(m_mutex);
	m_helps.push_back(std::make_pair(key, val));
}
bool Env::removeHelp(const std::string& key){
	MutexType::WriteLock lock(m_mutex);
	for(auto it = m_helps.begin(); it != m_helps.end()
	   ;++it){
		if(it->first == key){
			m_helps.erase(it);
			return true;
		}
	}
	return false;
}


std::string Env::getAbsolutePath(const std::string& path){
	if(path.empty()){
		return "/";
	}
	if(path[0] == '/'){
		return path;
	}
	return m_cwd + path;

}
void Env::printHlep(){
	MutexType::ReadLock lock(m_mutex); 
	std::cout<<"Usage: "<<"[options]"<<std::endl;
	for(auto it : m_helps){
		std::cout<<std::setw(5)<<"-"<<it.first <<" : "<<it.second<<std::endl;
	}
}

void Env::setEnv(const std::string& key,const std::string& val){
	setenv(key.c_str(),val.c_str(),0);
}
const std::string Env::getEnv(const std::string& key,const std::string& default_val){
	return getenv(key.c_str());
}

std::string Env::get(const std::string& key,const std::string& val){
	MutexType::ReadLock lock(m_mutex);
	auto it = m_args.find(key);
	return it != m_args.end() ? it->second : val;
}

}
