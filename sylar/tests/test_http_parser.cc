#include<iostream>
#include"../sylar/sylar.h"
#include "../http/http_parser.h"
#include "../sylar/log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
char test_request_data[] = "GET / HTTP/1.1\r\n"
				 "Host: www.baidu.com\r\n"
				 "Content-Length: 10\r\n\r\n"
				 "1234567789";
void test(){
	sylar::http::HttpRequestParser::ptr parser(new sylar::http::HttpRequestParser);
	std::string tmp =test_request_data;

	size_t s = parser->execute(&test_request_data[0], tmp.size());
	SYLAR_LOG_INFO(g_logger) << "execute rt: "<<s 
		  		 << " has Error"<<parser->hasError()
				 <<" isFinished: "<<parser->isFinished()
				 <<"tmp.size():" << tmp.size();

	SYLAR_LOG_INFO(g_logger) <<"to string:  "<<parser->getData()->toString();
	SYLAR_LOG_INFO(g_logger) <<"content-length:  "<<parser->getContentLength();
}

char test_response_data[] =  "HTTP/1.1 200 OK\r\n"
"Content-Encoding: gzip\r\n"
"content-length: 80\r\n"
"Content-Type: text/html;charset=utf-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html lang=\"en\">\r\n"
"<head>\r\n"
"<meta charset=\"UTF-8\" />\r\n"
"<title>Document</title>\r\n"
"</head>\r\n"
"<body>\r\n"
"<p>this is http response</p>\r\n"
"</body>\r\n"
"</html>\r\n";

void test_respinse(){
	sylar::http::HttpResponseParser::ptr parser(new sylar::http::HttpResponseParser);
	std::string tmp =test_response_data;

	size_t s = parser->execute(&test_response_data[0], tmp.size(),false);
	SYLAR_LOG_INFO(g_logger) << "execute rt: "<<s 
		  		 << " has Error"<<parser->hasError()
				 <<" isFinished: "<<parser->isFinished()
				 <<"tmp.size():" << tmp.size();

	SYLAR_LOG_INFO(g_logger) <<"to string:  "<<parser->getData()->toString();
	SYLAR_LOG_INFO(g_logger) <<"content-length:  "<<parser->getContentLength();
	tmp.resize(s);
	SYLAR_LOG_INFO(g_logger) <<"tmp--body: --"<< tmp;
}



int main(){
	sylar::IOManager iom(2);
	iom.scheduler(test);
	iom.scheduler(test_respinse);

}
