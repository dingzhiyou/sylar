#include "servlet.h"
#include "http.h"
#include <utility>
#include<fnmatch.h>


namespace sylar{
namespace http{

uint32_t FunctionServlet::handle(http::HttpRequest::ptr request,http::HttpResponse::ptr response,
			http::HttpSession::ptr session){
	return m_cb(request,response,session);
}

ServletDispatch::ServletDispatch():Servlet("ServletDispatch") {
		m_default.reset(new NotFoundServlet());
}
uint32_t ServletDispatch::handle(http::HttpRequest::ptr request,http::HttpResponse::ptr response,
			http::HttpSession::ptr session){
	auto slt = getMatchServlet(request->getPath());
	if(slt ){
		slt->handle(request,response,session);
	}
	return 0;
}

void ServletDispatch::addServlet(const std::string& uri,Servlet::ptr sl){
	RWMutex::WriteLock lock(m_mutex);
	m_datas[uri] = sl;
}
void ServletDispatch::addServlet(const std::string& uri,FunctionServlet::callback cb){
	RWMutex::WriteLock lock(m_mutex);
	m_datas[uri].reset(new FunctionServlet(cb));

}
void ServletDispatch::addGlobServlet(const std::string& uri,Servlet::ptr sl){
	RWMutex::WriteLock lock(m_mutex);
	for(auto it = m_globs.begin();it != m_globs.end();
			++it){
		if(it->first == uri){
			m_globs.erase(it);
			break;
		}
	}
	m_globs.push_back(std::make_pair(uri,sl));
}
void ServletDispatch::addGlobServlet(const std::string& uri,FunctionServlet::callback cb){
	addGlobServlet(uri,FunctionServlet::ptr(new FunctionServlet(cb)));
}

void ServletDispatch::delServlet(const std::string& uri){
	RWMutex::WriteLock lock(m_mutex);
	m_datas.erase(uri);
}
void ServletDispatch::delGlobServlet(const std::string& uri){
	RWMutex::WriteLock lock(m_mutex);
	for(auto it = m_globs.begin();it != m_globs.end();
			++it){
		if(it->first == uri){
			m_globs.erase(it);
			break;
		}
	}
}

Servlet::ptr ServletDispatch::getServlet(const std::string& uri) {
	RWMutex::ReadLock lock(m_mutex);
	return m_datas.find(uri) == m_datas.end() ? nullptr : m_datas[uri];
}
Servlet::ptr ServletDispatch::getGlobServlet(const std::string& uri){
	RWMutex::ReadLock lock(m_mutex);
	for(auto it = m_globs.begin();it != m_globs.end();
			++it){
		if(!fnmatch(it->first.c_str(), uri.c_str(),0)){
			return it->second;
		}
	}
	return nullptr;
}


Servlet::ptr ServletDispatch::getMatchServlet(const std::string& uri){
	RWMutex::ReadLock lock(m_mutex);
	auto it = m_datas.find(uri);
	if(it != m_datas.end()){
		return it->second;
	}
	for(auto it = m_globs.begin();it != m_globs.end();
			++it){
		if(!fnmatch(it->first.c_str(), uri.c_str(),0)){
			return it->second;
		}
	}
	return m_default;
}

NotFoundServlet::NotFoundServlet():Servlet("NotFoundServlet"){

}
uint32_t NotFoundServlet::handle(http::HttpRequest::ptr request,http::HttpResponse::ptr response,
				http::HttpSession::ptr session){
	
	static std::string RES_BODY = "404 Not Found";
	response->setStatus(sylar::http::HttpStatus::NOT_FOUND);
	response->setBody(RES_BODY);
	response->setHeader("Content-Type","text");
	return 0;
}




}
}
