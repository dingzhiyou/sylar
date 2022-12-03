#include "http_parser.h"
#include "http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"
#include "../sylar/log.h"
#include<string.h>

#define REQUEST_SIZE   4 * 1024
#define RESPONSE_SIZE  4 * 1024
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");


void on_request_method(void *data, const char *at, size_t length){
	sylar::http::HttpRequestParser* parse = static_cast<sylar::http::HttpRequestParser*>(data);
	sylar::http::HttpMethod m = sylar::http::CharsToHttpMethod(at);
	if(m == sylar::http::INVALID_METHOD){
		SYLAR_LOG_WARN(g_logger) << "Http INVALID_METHOD" << "  "<<std::string(at,length);
		parse->setError(1000);
		return;
	}
	parse->getData()->setMethod(m);
}
void on_request_uri(void *data, const char *at, size_t length){
	
}

void on_request_fragment(void *data, const char *at, size_t length){
	sylar::http::HttpRequestParser* parse = static_cast<sylar::http::HttpRequestParser*>(data);
	parse->getData()->setFragment(std::string(at,length));
}
void on_request_path(void *data, const char *at, size_t length){

	sylar::http::HttpRequestParser* parse = static_cast<sylar::http::HttpRequestParser*>(data);
	parse->getData()->setPath(std::string(at,length));
}
void on_request_query(void *data, const char *at, size_t length){

	sylar::http::HttpRequestParser* parse = static_cast<sylar::http::HttpRequestParser*>(data);
	parse->getData()->setQuery(std::string(at,length));
}
void on_request_version(void *data, const char *at, size_t length){
	sylar::http::HttpRequestParser* parse = static_cast<sylar::http::HttpRequestParser*>(data);
	uint8_t v;
	if(strncasecmp(at,"HTTP/1.1",length) == 0){
		v = 0x11;
		parse->getData()->setVersion(v);
		return;
	}else if(strncasecmp(at,"HTTP/1.0",length) == 0){
		v = 0x10;
		parse->getData()->setVersion(v);
		return;
	}
	parse->setError(1001);
	SYLAR_LOG_WARN(g_logger) << "invalid version";

}
void on_request_header_done(void *data, const char *at, size_t length){
	//sylar::http::HttpRequestParser* parse = static_cast<sylar::http::HttpRequestParser*>(data);

}
void on_request_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen){
	sylar::http::HttpRequestParser* parse = static_cast<sylar::http::HttpRequestParser*>(data);
	if(flen == 0){
		SYLAR_LOG_ERROR(g_logger) << "error field flen = "<<flen;
		return;
	}
	std::string key(field,flen);
	std::string val(value,vlen);
	parse->getData()->setHead(key, val);
}

namespace sylar{
namespace http{


HttpRequestParser::HttpRequestParser():m_error(0){
	m_data.reset(new HttpRequest);

	http_parser_init(&m_parser);
	m_parser.request_method = on_request_method;
	m_parser.request_uri = on_request_uri;
	m_parser.fragment = on_request_fragment;
	m_parser.request_path = on_request_path;
	m_parser.query_string = on_request_query;
	m_parser.http_version = on_request_version;
	m_parser.header_done = on_request_header_done;
	m_parser.http_field = on_request_http_field;
	m_parser.data = this;
}

uint32_t HttpRequestParser::getContentLength()const {
	return m_data->getHeaderAs<uint32_t>("content-length",0);
}


uint32_t HttpResponseParser::getContentLength()const {
	return m_data->getHeaderAs<uint32_t>("content-length",0);
}
int HttpRequestParser::isFinished(){
	return http_parser_finish(&m_parser);
}
size_t HttpRequestParser::execute(char* data,size_t len){
	size_t rt = http_parser_execute(&m_parser, data, len, 0);
	memmove(data,data + rt ,(len - rt ));
	return rt;
}
int HttpRequestParser::hasError(){
	return m_error || http_parser_has_error(&m_parser);
}

void on_response_reason(void *data, const char *at, size_t length){
	HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
	parser->getData()->setReason(std::string(at,length));
}

void on_response_status(void *data, const char *at, size_t length){
	HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
	HttpStatus status = (HttpStatus)(atoi(at));
	parser->getData()->setStatus(status);
}

void on_response_chunk(void *data, const char *at, size_t length){
	
}


void on_response_version(void *data, const char *at, size_t length){
	sylar::http::HttpResponseParser* parse = static_cast<sylar::http::HttpResponseParser*>(data);
	uint8_t v;
	if(strncasecmp(at,"HTTP/1.1",length) == 0){
		v = 0x11;
		parse->getData()->setVersion(v);
		return;
	}else if(strncasecmp(at,"HTTP/1.0",length) == 0){
		v = 0x10;
		parse->getData()->setVersion(v);
		return;
	}
	parse->setError(1001);
	SYLAR_LOG_WARN(g_logger) << "invalid version";


}

void on_response_last_chunk(void *data, const char *at, size_t length){

}


void on_response_header_done(void *data, const char *at, size_t length){

}

void on_response_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen){
	sylar::http::HttpResponseParser* parse = static_cast<sylar::http::HttpResponseParser*>(data);
	if(flen == 0){
		SYLAR_LOG_WARN(g_logger) << "error field flen = "<<flen;
		//parse->setError(1002);
		return;
	}
	std::string key(field,flen);
	std::string val(value,vlen);
	parse->getData()->setHeader(key, val);
}
HttpResponseParser::HttpResponseParser():m_error(0){
	m_data.reset(new HttpResponse);
	httpclient_parser_init(&m_parser);

	m_parser.http_field= on_response_http_field;
	m_parser.reason_phrase = on_response_reason;
	m_parser.status_code = on_response_status;
	m_parser.chunk_size = on_response_chunk;
	m_parser.http_version = on_response_version;
	m_parser.header_done = on_response_header_done;
	m_parser.last_chunk = on_response_last_chunk;
	m_parser.data = this;

}
int HttpResponseParser::isFinished(){
	return httpclient_parser_finish(&m_parser);
}
size_t HttpResponseParser::execute(char* data,size_t len,bool chunck){
	if(chunck){
		httpclient_parser_init(&m_parser);
	}
	size_t rt = httpclient_parser_execute(&m_parser, data, len, 0);
	memmove(data,data + rt ,(len - rt ));
	return rt;
}
int HttpResponseParser::hasError(){
	return m_error || httpclient_parser_has_error(&m_parser);

}




}

}
