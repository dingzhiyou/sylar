#ifndef __SYLAR_SERVLET_H__
#define __SYLAR_SERVLET_H__

#include "../sylar/thread.h"
#include "http.h"
#include <boost/type.hpp>
#include <functional>
#include<memory>
#include<unordered_map>
#include<vector>
#include"http_session.h"

namespace sylar{
namespace http{

class Servlet{
public:
	typedef std::shared_ptr<Servlet> ptr;
	Servlet(const std::string& name) :m_name(name){}
	virtual ~ Servlet(){}

	virtual uint32_t handle(http::HttpRequest::ptr request,http::HttpResponse::ptr response,
				http::HttpSession::ptr session) = 0;
private:
	std::string m_name;
};

class FunctionServlet : public Servlet{
public:
	typedef std::shared_ptr<FunctionServlet> ptr;
	typedef std::function<int32_t (http::HttpRequest::ptr request,http::HttpResponse::ptr response,
				http::HttpSession::ptr session)> callback;


	virtual uint32_t handle(http::HttpRequest::ptr request,http::HttpResponse::ptr response,
				http::HttpSession::ptr session) override;
	FunctionServlet(callback cb)
			:Servlet("FunctionServlet"),m_cb(cb){}


private:
	callback m_cb;
};


class NotFoundServlet;
class ServletDispatch : public Servlet{
public:
	typedef std::shared_ptr<ServletDispatch> ptr;
	ServletDispatch();

	virtual uint32_t handle(http::HttpRequest::ptr request,http::HttpResponse::ptr response,
				http::HttpSession::ptr session) override;

	void addServlet(const std::string& uri,Servlet::ptr sl);
	void addServlet(const std::string& uri,FunctionServlet::callback cb);
	void addGlobServlet(const std::string& uri,Servlet::ptr sl);
	void addGlobServlet(const std::string& uri,FunctionServlet::callback cb);

	void delServlet(const std::string& uri);
	void delGlobServlet(const std::string& uri);
	
	Servlet::ptr getServlet(const std::string& uri) ;
	Servlet::ptr getGlobServlet(const std::string& uri) ;

	void setDefaultServlet(Servlet::ptr sl) {m_default =sl;}
	Servlet::ptr getDefaultServlet()const {return m_default;}

	Servlet::ptr getMatchServlet(const std::string& uri) ;
private:
	RWMutex m_mutex;
	std::unordered_map<std::string,Servlet::ptr> m_datas;
	std::vector<std::pair<std::string,Servlet::ptr> > m_globs;
	Servlet::ptr m_default;

};

class NotFoundServlet : public Servlet{
public:
	typedef std::shared_ptr<NotFoundServlet> ptr;
	NotFoundServlet();
	virtual uint32_t handle(http::HttpRequest::ptr request,http::HttpResponse::ptr response,
				http::HttpSession::ptr session) override;
private:

};

}

}


#endif
