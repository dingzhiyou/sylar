#include "http_session.h"
#include"../sylar/macro.h"
#include "http_parser.h"
#include "../sylar/log.h"
#include <cstring>
#include <string>

namespace sylar{
namespace http{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

HttpSession::HttpSession(Socket::ptr sock,bool own):SockStream(sock,own){
}

HttpRequest::ptr HttpSession::rcvRequest(){
	HttpRequestParser::ptr parse(new HttpRequestParser);
	int buf_size = 4096;
	std::shared_ptr<char> buffer(new char[buf_size],[](char* ptr){
				delete[] ptr;
	});

	char* data = buffer.get();
	int offset = 0;
	do{	
		int len = read(data + offset,buf_size - offset);
		if(len <= 0){
			close();
			return nullptr;
		}
		len += offset;
		size_t nparse = parse->execute(data,len);
		if(parse->hasError()){
			close();
			return nullptr;
		}
		offset = len - nparse;
		if(offset == buf_size){
			close();
			return nullptr;
		}
		if(parse->isFinished()){
			break;
		}
	}while(true);

	int32_t length = parse->getContentLength();
	if(length > 0){
		std::string body;
		body.resize(length);
		
		int len = 0;
		if(length >= offset){
			std::memcpy(&body[0],data,offset);
			len =offset;
		}else{
			std::memcpy(&body[0],data,length);
			len = length;	
		}
		length -= len;

		
		int rt;
		if(length > 0){
			if((rt = readFix(&body[len],length)) <= 0){
				SYLAR_ASSERT(rt == length);
				close();
				return nullptr;
			}	
		}
		parse->getData()->setBody(body);
	}
	return  parse->getData();
}
int HttpSession::sendResponse(HttpResponse::ptr rsq){
	std::string str = rsq->toString();
	int rt = writeFix(str.c_str(),str.size());
	return rt;
}

}


}
