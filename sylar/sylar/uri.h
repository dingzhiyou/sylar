#ifndef  __SYLAR_URI_H__
#define __SYLAR_URI_H__
#include<memory>
#include <ostream>
#include <sstream>
#include "ipaddr.h"
namespace sylar{

class Uri{
public:
	typedef std::shared_ptr<Uri> ptr;
	static Uri::ptr Create(const std::string& uri);
	std::string toString()const ;
	std::ostream& dump(std::ostream& os)const;
	
	Uri();

	const std::string& getScheme()const{return m_scheme;}
	const std::string& getUserinfo()const{return m_userinfo;}
	const std::string& getHost()const{return m_host;}
	const std::string& getPath()const;
	const std::string& getQuery()const{return m_query;}
	const std::string& getFragment()const{return m_fragment;}
	int32_t getPort()const;

	void setScheme(const std::string& scheme){m_scheme = scheme;}
	void setUserinfo(const std::string& userinfo){m_userinfo = userinfo;}
	void setHost(const std::string& host){m_host = host;}
	void setPath(const std::string& path){m_path = path;}
	void setQuery(const std::string& query){m_query = query;}
	void setFragment(const std::string& fragment){m_fragment = fragment;}
	void setPort(int32_t port){m_port = port;}
	Address::ptr createAddress()const;
	bool isDefaultPort()const;
private:

	std::string m_scheme;
	std::string m_userinfo;
	std::string m_host;
	std::string m_path;
	std::string m_query;
	std::string m_fragment;
	int32_t m_port;

};


}


#endif
