#ifndef __SYLAR_HTTP_PARSER_H__
#define __SYLAR_HTTP_PARSER_H__

#include<memory>
#include "http11_parser.h"
#include "httpclient_parser.h"
#include "http.h"
namespace sylar{
namespace http{
class HttpRequestParser{
public:
	typedef std::shared_ptr<HttpRequestParser> ptr;
	HttpRequestParser();


	int isFinished();
	size_t execute(char* data,size_t len);
	int hasError();
	
	HttpRequest::ptr getData()const {return m_data;}
	void setError(int error) {m_error = error;}
	uint32_t getContentLength()const;


	const http_parser& getParse()const {return  m_parser;}
private:
	http_parser m_parser;
	HttpRequest::ptr m_data;
	int m_error;
};

class HttpResponseParser{
public:
	typedef std::shared_ptr<HttpResponseParser> ptr;
	HttpResponseParser();

	int isFinished();
	size_t execute(char* data,size_t len,bool chunck = false);
	int hasError();
	void setError(int v){m_error = v;}
	HttpResponse::ptr getData()const {return m_data;}
	uint32_t getContentLength()const;
	const httpclient_parser& getParse()const{return  m_parser;}
private:
	httpclient_parser m_parser;
	HttpResponse::ptr m_data;
	int m_error;
};



}

}


#endif
