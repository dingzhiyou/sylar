#ifndef __SYARL_APPLICATION_H__
#define __SYARL_APPLICATION_H__

#include <memory>
#include <vector>
#include "../http/http_server.h"

namespace sylar{

class Application{
public:
	typedef std::shared_ptr<Application> ptr;
	Application();
	bool init(int argc,char** argv);
	bool run();
	int main(int argc,char** argv);
	int run_fiber();
private:
	static Application* s_instance;
	int m_argc;
	char** m_argv;
	std::vector<http::HttpServer::ptr> m_http_servers;

};

}

#endif
