#include "http_server.h"
#include "../sylar/log.h"
#include "http.h"
#include "http_session.h"
#include "servlet.h"
#include <cstring>

namespace sylar{
namespace http{
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");


HttpServer::HttpServer(bool keepalive ,IOManager* worker,IOManager* acceptWorker):TcpServer(worker,acceptWorker),m_isKeepalive(keepalive){
	m_dispatch.reset(new ServletDispatch());       
	 }

void HttpServer::handleClient(Socket::ptr sock){
 	HttpSession::ptr session(new HttpSession(sock));
	HttpRequest::ptr req = nullptr;
	do{
		req = session->rcvRequest();
		if(!req){
			SYLAR_LOG_WARN(g_logger) << "session->rcvRequest err"<<" errno:" <<errno <<"errstr" <<" "<<strerror(errno);
			return;
		}

		HttpResponse::ptr rsp(new HttpResponse(req->getVersion(),req->isClose() || !m_isKeepalive));
		//不一定命中
		m_dispatch->handle(req,rsp,session);
		session->sendResponse(rsp);
	}while(m_isKeepalive && !req->isClose());
	session->close();
}

}

}

