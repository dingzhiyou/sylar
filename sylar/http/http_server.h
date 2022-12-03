#ifndef __SYLAR_HTTP_SERVER_H__
#define __SYLAR_HTTP_SERVER_H__
#include "../sylar/tcp_server.h"
#include <memory>
#include "servlet.h"
namespace sylar{
namespace http{

class HttpServer :public TcpServer{
public:
	typedef std::shared_ptr<HttpServer> ptr;
	HttpServer(IOManager* worker = sylar::IOManager::GetThis(),IOManager* acceptWorker = sylar::IOManager::GetThis(),bool keepalive = false);
	virtual void handleClient(Socket::ptr sock) override;
	void setDispatch(ServletDispatch::ptr disptch){m_dispatch = disptch;}
	ServletDispatch::ptr getDispatch()const {return m_dispatch;}
private:
	bool m_isKeepalive;
	ServletDispatch::ptr m_dispatch;
};


}

}

#endif
