#include "../http/http_server.h"
#include "../sylar/sylar.h"


void run(){
	sylar::http::HttpServer::ptr http_server(new sylar::http::HttpServer);
	sylar::Address::ptr addr = sylar::Address::LookupAny("127.0.0.1:8020");
	
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


	http_server->start();

}

int main(){
	sylar::IOManager iom(2);
	iom.scheduler(run);

	return 0;
}
