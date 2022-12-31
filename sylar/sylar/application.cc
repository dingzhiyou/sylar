#include"application.h"
#include "../http/http_server.h"
#include "ipaddr.h"
#include "iomanager.h"
#include "config.h"
#include "env.h"
#include "log.h"
#include "daemon.h"
#include "util.h"
#include <cerrno>
#include <cstring>
#include <fstream>
#include <functional>
#include <ios>
#include <memory>
#include <sstream>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <iostream>
namespace sylar{

class Init_Log_FileAppender{
public:
	Init_Log_FileAppender(const std::string& log_path){
		FileLogAppender::ptr file_log(new FileLogAppender(log_path));
		g_logger->addAppender(file_log);
	}
};



struct HttpServerConf{
	std::vector<std::string> address;
	int keepalive = 0;
	int timeout = 1000 * 60 *2;
	std::string name;

	bool isInvalid(){
		return !address.empty();
	}
	bool operator==(const HttpServerConf& oth)const{
		return address == oth.address
			&& keepalive == oth.keepalive
			&& timeout == oth.timeout
			&& name == oth.name;
	}
};

template<>
class LexicalCast<std::string, HttpServerConf>{
public:
	HttpServerConf operator()(const std::string& v){
		YAML::Node node = YAML::Load(v);
		HttpServerConf conf;
		conf.keepalive = node["keepalive"].as<int>(conf.keepalive);
		conf.timeout = node["timeout"].as<int>(conf.timeout);
		conf.name = node["name"].as<std::string>(conf.name);
		if(node["address"].IsDefined()){
			for(size_t i = 0; i < node["address"].size();i++){
				conf.address.push_back(node["address"][i].as<std::string>());
			}
		}
		return conf; 
	}
};

template<>
class LexicalCast<HttpServerConf, std::string>{
public:
	std::string operator()(const HttpServerConf& conf){
		YAML::Node node;

		node["name"] = conf.name;
		node["keepalive"] = conf.keepalive;
		node["timeout"] = conf.timeout;
		for(auto& i : conf.address){
			node["address"].push_back(i);
		}
		std::stringstream ss;
		ss << node;
		return ss.str();
	}
};


static sylar::ConfigVar<std::string>::ptr g_server_work_path = 
	sylar::Config::Lookup("server.work_path",std::string("/apps/work/sylar"),"server work path");
static sylar::ConfigVar<std::string>::ptr g_server_pid_file = 
	sylar::Config::Lookup("server.pid_file",std::string("sylar.pid"),"server pid file");

static sylar::ConfigVar<std::vector<HttpServerConf> > ::ptr g_http_servers = 
	sylar::Config::Lookup("http_servers",std::vector<HttpServerConf>(),"http servers");

Init_Log_FileAppender init_log(g_server_work_path->getValue() + "/" + "sylar.log");

//static sylar::Logger::ptr g_logger1 = SYLAR_LOG_NAME("system");
//g_logger1->addAppender(new FileLogAppender(g_server_work_path->getValue() + ".log"));
Application* Application::s_instance = nullptr;	
Application::Application(){
	s_instance = this;
}
bool Application::init(int argc, char **argv){
	m_argc = argc;
	m_argv = argv;
	SYLAR_LOG_DEBUG(g_logger) << "in application init";
	sylar::EnvMgr::GetInstance()->addHelp("s","start with the terinal");
	sylar::EnvMgr::GetInstance()->addHelp("d","run as deamon");
	sylar::EnvMgr::GetInstance()->addHelp("c","conf path default ./conf");
	sylar::EnvMgr::GetInstance()->addHelp("p","print help");

	if(!sylar::EnvMgr::GetInstance()->init(argc,argv)){
		sylar::EnvMgr::GetInstance()->printHlep();
		SYLAR_LOG_DEBUG(g_logger) << "in application init";
		return false;
	}

	int run_type = 0;
	if(sylar::EnvMgr::GetInstance()->has("s")){
		run_type = 1;
	}
	if(sylar::EnvMgr::GetInstance()->has("d")){
		run_type = 2;
	}
	if(run_type == 0){
		sylar::EnvMgr::GetInstance()->printHlep();
		SYLAR_LOG_DEBUG(g_logger) << "in application init";
		return false;
	}
	return true;
}

bool Application::run(){
	bool is_daemo = sylar::EnvMgr::GetInstance()->has("d");
	return  start_daemon(m_argc, m_argv,std::bind(&Application::main,this,std::placeholders::_1,std::placeholders::_2),is_daemo);
}

int Application::main(int argc, char **argv){


	std::string pidfile = g_server_work_path->getValue() + "/" + g_server_pid_file->getValue();

	if(!FSUtil::IsRunningPidFile(pidfile)){
		SYLAR_LOG_ERROR(g_logger) << "server is running: "<<pidfile;
		return false;
	}

	std::string conf_path  = sylar::EnvMgr::GetInstance()->getAbsolutePath(sylar::EnvMgr::GetInstance()->get("c","conf"));
	SYLAR_LOG_DEBUG(g_logger) << "load conf path: " <<conf_path;
	sylar::Config::LoadFromConDir(conf_path);

	std::ofstream ofs;
	ofs.open(pidfile,std::ios::trunc);
	if(!ofs){
		SYLAR_LOG_ERROR(g_logger) << "pidfile: "<<  pidfile<< " open faild"<<"  errno:"<<errno<<"  errstr:"<<strerror(errno);
		return false;
	}
	ofs << getpid();
	ofs.close();
	sylar::IOManager iom(2);
	iom.scheduler(std::bind(&Application::run_fiber,this));
	iom.stop();
	return 0;
}

int Application::run_fiber(){
	auto http_conf = g_http_servers->getValue();
	for(auto& i : http_conf){
		SYLAR_LOG_DEBUG(g_logger) << LexicalCast<HttpServerConf,std::string>()(i);
		std::vector<Address::ptr> address;
	        for(auto& x: i.address){
			size_t pos = x.find(":");
			if(pos == std::string::npos){
				SYLAR_LOG_ERROR(g_logger)<< " error ipaddress: "<<x;
				continue;
			}
			auto addr = sylar::Address::LookupAny(x);
			if(addr){
				address.push_back(addr);
				continue;
			}
			std::vector<std::pair<Address::ptr,uint32_t> > result;
			if(!sylar::Address::GetInterfaceAddress(result,x.substr(0,pos))){
				SYLAR_LOG_ERROR(g_logger) << "invalid address";
				continue;	
			}
			for(auto& a:result){
				auto ipaddr = std::dynamic_pointer_cast<IPAddress>(a.first);
				if(ipaddr){
					ipaddr->setPort(atoi(x.substr(pos + 1).c_str()));
					address.push_back(ipaddr);
				}
			}


		}

		sylar::http::HttpServer::ptr server(new sylar::http::HttpServer(i.keepalive));
		std::vector<Address::ptr> failds;
		server->bind(address,failds);
		if(!failds.empty()){
			for(auto& a : failds){
				SYLAR_LOG_ERROR(g_logger) << a->toString();
			}
			return  -1;
		}
		server->setRecvTimeout(i.timeout);
		server->start();
		m_http_servers.push_back(server);
	}
	return 0;
}


}

