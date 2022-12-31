#include "../http/http_server.h"
#include "../sylar/sylar.h"
#include "../http/resourceServlet.h"
#include "../sylar/env.h"

void run(){
	sylar::http::HttpServer::ptr http_server(new sylar::http::HttpServer(true));
	sylar::Address::ptr addr = sylar::Address::LookupAny("192.168.94.132:8020");
	
	SYLAR_LOG_DEBUG(SYLAR_LOG_ROOT()) <<"m_family："<<addr->getFamily();

	http_server->bind(addr);
	auto dispatch = http_server->getDispatch();
	dispatch->addServlet("/dzy",[](sylar::http::HttpRequest::ptr req,sylar::http::HttpResponse::ptr rsp,sylar::http::HttpSession::ptr session){
		rsp->setBody("Hello Dzy");
		return 0;		
	});

	dispatch->addGlobServlet("/dzy/*",[](sylar::http::HttpRequest::ptr req,sylar::http::HttpResponse::ptr rsp,sylar::http::HttpSession::ptr session){
		rsp->setBody(req->toString());
		return 0;		
	});
	dispatch->addGlobServlet("/html/*",sylar::http::Servlet::ptr(new sylar::http::ResourceServlet(sylar::EnvMgr::GetInstance()->getCWD())));


	http_server->start();

}

int main(int argc,char** argv){
	sylar::EnvMgr::GetInstance()->init(argc,argv);
	sylar::IOManager iom(2);
	iom.scheduler(run);

	return 0;
}
