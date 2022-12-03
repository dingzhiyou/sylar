#ifndef __SYLAR_HTTP_CONNECTION_H__
#define __SYLAR_HTTP_CONNECTION_H__
#include "../sylar/sockstream.h"
#include "http.h"
#include<atomic>
#include"../sylar/uri.h"
#include <boost/type.hpp>
#include "../sylar/thread.h"
#include<list>
#include <map>
#include <memory>
namespace sylar{
namespace http{

struct HttpResult{
	typedef std::shared_ptr<HttpResult> ptr;

	enum Error{
		OK = 1,
		INVALID_URL = 2,
		INVALID_HOST = 3,
		CONNECT_FAILD = 4,
		SEND_CLOSE_BY_PEER = 5,
		SEND_SOCK_ERROR = 6,
		RCVRESPONSE_TIMEOUT = 7,
		GET_CONNECTION_POOL = 8,
		INVALID_CONNECTION = 9,
	};
	HttpResult(int _result,HttpResponse::ptr _response,std::string _error)
		   :result(_result)
		   ,response(_response)
		   ,error(_error){
		   
		   }

	std::string toString()const;
	int result;
	HttpResponse::ptr response;
	std::string error;

};

class HttpConnectionPool;

class HttpConnection : public SockStream{
friend class HttpConnectionPool;

public:
	typedef std::shared_ptr<HttpConnection> ptr;
	HttpConnection(Socket::ptr,bool own = false);

	
	static HttpResult::ptr DoGet(const std::string& url,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");
	static HttpResult::ptr DoGet(Uri::ptr uri,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");


	static HttpResult::ptr DoPost(const std::string& url,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");

	static HttpResult::ptr DoPost(Uri::ptr uri,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");
	static HttpResult::ptr DoRequest(HttpMethod method,const std::string& url,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");


	static HttpResult::ptr DoRequest(HttpMethod method,Uri::ptr uri,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");
	
	static HttpResult::ptr DoRequest(HttpRequest::ptr,Uri::ptr uri,uint64_t time_out);


	HttpResponse::ptr rcvResponse();
	int sendRequest(HttpRequest::ptr rsq);	

private:
	uint64_t m_createTime = 0;

};


class HttpConnectionPool{
public:
	typedef std::shared_ptr<HttpConnectionPool> ptr;

	typedef Mutex MutexType;

	HttpConnectionPool(const std::string& host
			  ,const std::string& vhost
			  ,const int32_t port
			  ,const int32_t max_size
			  ,const int32_t max_alivetime
			  ,const int32_t max_request);

	HttpConnection::ptr getConnection();


	HttpResult::ptr doGet(const std::string& url,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");
	HttpResult::ptr doGet(Uri::ptr uri,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");


	HttpResult::ptr doPost(const std::string& url,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");

	HttpResult::ptr doPost(Uri::ptr uri,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");
	HttpResult::ptr doRequest(HttpMethod method,const std::string& url,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");


	HttpResult::ptr doRequest(HttpMethod method,Uri::ptr uri,uint64_t time_out,
					 std::map<std::string, std::string> headers = {},const std::string& body = "");
	
	HttpResult::ptr doRequest(HttpRequest::ptr,uint64_t time_out);



private:
	static void ReleasePtr(HttpConnection* ptr,HttpConnectionPool* pool);


private:
	std::string m_host;
	std::string m_vhost;
	int32_t m_port;
	int32_t m_maxSize;
	int32_t m_maxAliveTime;
	int32_t m_maxRequest;
	MutexType m_mutex;
	std::list<HttpConnection*> m_conns;
	std::atomic<int32_t> m_total = {0};



};





}
}

#endif
