#include "http_connection.h"
#include"../sylar/macro.h"
#include "http.h"
#include "http_parser.h"
#include "../sylar/log.h"
#include <boost/type.hpp>
#include <cerrno>
#include"../sylar/thread.h"
#include <cstring>
#include <ctime>
#include"../sylar/ipaddr.h"
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <strings.h>

namespace sylar{
namespace http{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");


std::string HttpResult::toString()const{
	std::stringstream ss;
	ss << " result: "<<std::to_string(result)
	   << " error: "<<error;
	if(response){
		ss << response->toString();
	}else{
		ss << "nullptr";
		SYLAR_ASSERT(false);
	}
	return ss.str();
}

HttpConnection::HttpConnection(Socket::ptr sock,bool own):SockStream(sock,own){
}
	
HttpResult::ptr HttpConnection::DoGet(const std::string& url,uint64_t time_out,
 		std::map<std::string, std::string> headers,const std::string& body){
	
	return DoRequest(HttpMethod::GET,url,time_out,headers,body);

}
HttpResult::ptr HttpConnection::DoGet(Uri::ptr uri,uint64_t time_out,
 		std::map<std::string, std::string> headers,const std::string& body){

	return DoRequest(HttpMethod::GET,uri,time_out,headers,body);
}


HttpResult::ptr HttpConnection::DoPost(const std::string& url,uint64_t time_out,
 		std::map<std::string, std::string> headers,const std::string& body){

	return DoRequest(HttpMethod::POST,url,time_out,headers,body);

}

HttpResult::ptr HttpConnection::DoPost(Uri::ptr uri,uint64_t time_out,
 		std::map<std::string, std::string> headers,const std::string& body){

	return DoRequest(HttpMethod::POST,uri,time_out,headers,body);
}
HttpResult::ptr HttpConnection::DoRequest(HttpMethod method,const std::string& url,uint64_t time_out,
 		std::map<std::string, std::string> headers,const std::string& body){
		Uri::ptr uri = Uri::Create(url);
		if(!uri){
			return std::make_shared<HttpResult>(HttpResult::Error::INVALID_URL,nullptr,"invalid_url");
		}
		

		return DoRequest(method,uri,time_out,headers,body);

}
HttpResult::ptr HttpConnection::DoRequest(HttpMethod method,Uri::ptr uri,uint64_t time_out,
 		std::map<std::string, std::string> headers,const std::string& body){

		HttpRequest::ptr req(new HttpRequest());
		req->setMethod(method);
		
		bool has_host = false;
		for(auto& i : headers){
			if(strcasecmp(i.first.c_str(), "connection") == 0){
				if(strcasecmp(i.second.c_str(), "keep-alive") == 0){
					req->setClose(false);
				}
				continue;
			}
			if(strcasecmp(i.first.c_str(),"host") == 0){
				has_host = !i.second.empty();
			}

			req->setHead(i.first,i.second);
		}

		if(!has_host){
			req->setHead("Host", uri->getHost());
		}
		req->setBody(body);
		req->setQuery(uri->getQuery());
		req->setPath(uri->getPath());
		req->setFragment(uri->getFragment());
		
		return DoRequest(req,uri,time_out);

}

HttpResult::ptr HttpConnection::DoRequest(HttpRequest::ptr rsq,Uri::ptr uri,uint64_t time_out){
	sylar::Address::ptr addr = uri->createAddress();
	if(!addr){
		return std::make_shared<HttpResult>(HttpResult::Error::INVALID_HOST,nullptr,"invalid_host");
	}
	Socket::ptr sock = Socket::CreateTCP(addr);
	if(!sock){
		return std::make_shared<HttpResult>(HttpResult::Error::INVALID_HOST,nullptr,"invalid_faild" + addr->toString());
	}
	sock->connect(addr);
	sock->setRecvTimerout(time_out);
	
	HttpConnection::ptr conn(new HttpConnection(sock));
	int rt = conn->sendRequest(rsq);
	if(rt == 0){
		return std::make_shared<HttpResult>(HttpResult::Error::SEND_CLOSE_BY_PEER,nullptr,
				"sendclose by peer" + sock->getRemoteAddress()->toString());
		
	}
	if(rt < 0){
		return std::make_shared<HttpResult>(HttpResult::Error::SEND_SOCK_ERROR,nullptr,
				"send sock error errno = " + std::to_string(errno) +" errstr :" + std::string(strerror(errno)));
	}
	HttpResponse::ptr rsp = conn->rcvResponse();
	if(!rsp){
		return std::make_shared<HttpResult>(HttpResult::Error::RCVRESPONSE_TIMEOUT,nullptr,
				"rcv time_out" + std::string("remoteAddr : ") + sock->getRemoteAddress()->toString());
	}
	return std::make_shared<HttpResult>(HttpResult::Error::OK,rsp,"ok");
}
HttpResponse::ptr HttpConnection::rcvResponse(){
	HttpResponseParser::ptr parse(new HttpResponseParser);
	int buf_size = 4096;
	std::shared_ptr<char> buffer(new char[buf_size + 1],[](char* ptr){
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
		data[len] = '\0';
		size_t nparse = parse->execute(data,len,false);
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
	
	auto client_parse = parse->getParse();
	if(client_parse.chunked){
		std::string body;
		int len = offset;
		do{
			do{
				int rt = read(data + len,buf_size - len);
				if(rt <= 0){
					close();
					SYLAR_LOG_DEBUG(g_logger)<<"0000000000000000";
					return nullptr;
				}
				len += rt;
				data[len] = '\0';
				size_t nparse = parse->execute(data,len,true);
				if(parse->hasError()){
					close();
					SYLAR_LOG_DEBUG(g_logger)<<"0000000000000000";
					return nullptr;
				}
				len -= nparse;
				if(len == int(buf_size)){
					close();
					SYLAR_LOG_DEBUG(g_logger)<<"0000000000000000";
					return nullptr;
				}
			}while(!parse->isFinished());
			len -= 2;
			if(client_parse.content_len <= len){
				body.append(data,client_parse.content_len);
				std::memmove(data,data + client_parse.content_len,len-client_parse.content_len);
				len -= client_parse.content_len;
			}else{
				body.append(data,len);
				int left = client_parse.content_len - len;
				while(left > 0){
					int rt = read(data,buf_size > left ? left : buf_size);
					if(rt <=0){
						close();
					SYLAR_LOG_DEBUG(g_logger)<<"0000000000000000";
						return nullptr;	
					}
					body.append(data,rt);
					left -= rt;
				}
				len = 0;
			}
		}while(!client_parse.chunks_done);
		parse->getData()->setBody(body);
	}else{
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
					SYLAR_LOG_DEBUG(g_logger)<<"0000000000000000";
					return nullptr;
				}	
			}
			parse->getData()->setBody(body);
		}
	}
	SYLAR_LOG_DEBUG(g_logger) <<"parse.use_count: "<< parse.use_count();
	return  parse->getData();
}
int HttpConnection::sendRequest(HttpRequest::ptr rsq){
	std::string str = rsq->toString();
	int rt = writeFix(str.c_str(),str.size());
	return rt;
}



HttpConnectionPool::HttpConnectionPool(const std::string& host
		  ,const std::string& vhost
		  ,const int32_t port
		  ,const int32_t max_size
		  ,const int32_t max_alivetime
		  ,const int32_t max_request)
		  :m_host(host)
		  ,m_vhost(vhost)
		  ,m_port(port)
		  ,m_maxSize(max_size)
		  ,m_maxAliveTime(max_alivetime)
		  ,m_maxRequest(max_request){


}

HttpConnection::ptr HttpConnectionPool::getConnection(){
	MutexType::Lock lock(m_mutex);
	HttpConnection* ptr = nullptr;
	uint64_t now_ms  = GetCurrentMs();
	while(!m_conns.empty()){
		ptr = nullptr;
		auto conn = *m_conns.begin();
		m_conns.pop_front();
		if(!conn->isConnected()){	
			delete conn;
			m_total--;
			continue;
		}
		if(conn->m_createTime + m_maxAliveTime > now_ms){
			delete conn;
			m_total--;
			continue;
		}
		ptr = conn;
		break;
	}

	lock.unlock();
	if(!ptr){
		sylar::IPAddress::ptr addr = sylar::IPAddress::LookupAnyIPAddress(m_host);
		if(!addr){
			SYLAR_LOG_WARN(g_logger) << "m_host to addr error";
			return nullptr;
		}
		addr->setPort(m_port);
		Socket::ptr sock = Socket::CreateTCP(addr);
		if(!sock){
			SYLAR_LOG_WARN(g_logger) << "addr to sock error";
			return nullptr;
		}
		if(!sock->connect(addr)){
			SYLAR_LOG_WARN(g_logger) << "connect to addr error";
			return nullptr;
		}
		
		ptr = (new HttpConnection(sock));
		++m_total;
	}

	return HttpConnection::ptr(ptr,std::bind(&HttpConnectionPool::ReleasePtr,
					   std::placeholders:: _1,this));
}




void HttpConnectionPool::ReleasePtr(HttpConnection* ptr,HttpConnectionPool* pool){
	uint64_t now_ms = sylar::GetCurrentMs();;
	if(!ptr->isConnected()){
		delete ptr;
		--pool->m_total;
		return;
	}
	if(ptr->m_createTime + pool->m_maxAliveTime > now_ms){
		delete ptr;
		--pool->m_total;
		return;
	}
	MutexType::Lock   lock(pool->m_mutex);
	pool->m_conns.push_back(ptr);
	return;
}

HttpResult::ptr HttpConnectionPool::doGet(const std::string& url,uint64_t time_out,
				 std::map<std::string, std::string> headers,const std::string& body){
		return doRequest(HttpMethod::GET,url,time_out,headers,body);
}
HttpResult::ptr HttpConnectionPool::doGet(Uri::ptr uri,uint64_t time_out,
				 std::map<std::string, std::string> headers,const std::string& body){
	std::stringstream ss;
	ss << uri->getPath()
	   << (uri->getQuery().empty() ? "" : "?")
	   <<uri->getQuery()
	   <<(uri->getFragment().empty() ? "" : "#")
	   <<uri->getFragment();

	return doGet(ss.str(),time_out,headers,body);
}


HttpResult::ptr HttpConnectionPool::doPost(const std::string& url,uint64_t time_out,
				 std::map<std::string, std::string> headers,const std::string& body){
		return doRequest(HttpMethod::POST,url,time_out,headers,body);
}

HttpResult::ptr HttpConnectionPool::doPost(Uri::ptr uri,uint64_t time_out,
				 std::map<std::string, std::string> headers,const std::string& body){

	std::stringstream ss;
	ss << uri->getPath()
	   << (uri->getQuery().empty() ? "" : "?")
	   <<uri->getQuery()
	   <<(uri->getFragment().empty() ? "" : "#")
	   <<uri->getFragment();

	return doPost(ss.str(),time_out,headers,body);

}
HttpResult::ptr HttpConnectionPool::doRequest(HttpMethod method,const std::string& url,uint64_t time_out,
				 std::map<std::string, std::string> headers,const std::string& body){

	HttpRequest::ptr req(new HttpRequest());
		req->setMethod(method);
		
		bool has_host = false;
		for(auto& i : headers){
			//if(strcasecmp(i.first.c_str(), "connection") == 0){
			//	if(strcasecmp(i.second.c_str(), "keep-alive") == 0){
			//		req->setClose(false);
			//	}
			//	continue;
			//}
			if(strcasecmp(i.first.c_str(),"host") == 0){
				has_host = !i.second.empty();
			}

			req->setHead(i.first,i.second);
		}

		if(!has_host){
			if(m_vhost.empty()){
				req->setHead("Host",m_host);
			}else{
				req->setHead("Host",m_vhost);
			}
		}
		req->setBody(body);
		req->setPath(url);
		
		return doRequest(req,time_out);
}


HttpResult::ptr HttpConnectionPool::doRequest(HttpMethod method,Uri::ptr uri,uint64_t time_out,
				 std::map<std::string, std::string> headers,const std::string& body){

	std::stringstream ss;
	ss << uri->getPath()
	   << (uri->getQuery().empty() ? "" : "?")
	   <<uri->getQuery()
	   <<(uri->getFragment().empty() ? "" : "#")
	   <<uri->getFragment();

	return doRequest(method,ss.str(),time_out,headers,body);
}

HttpResult::ptr HttpConnectionPool::doRequest(HttpRequest::ptr rsq,uint64_t time_out){
	rsq->setClose(false);
	for(auto it = rsq->getHeaders().begin();it != rsq->getHeaders().end();
			++it){
		SYLAR_LOG_WARN(g_logger) << it->first<<"   "<<it->second;
	}
	auto conn = getConnection();
	if(!conn){
		return std::make_shared<HttpResult>(HttpResult::Error::GET_CONNECTION_POOL,nullptr
				 		   ,"get connection faild" + m_host + " port " + std::to_string(m_port));
	}

	Socket::ptr sock = conn->getSocket();
	if(!sock){
		return std::make_shared<HttpResult>(HttpResult::Error::INVALID_CONNECTION,nullptr,"invalid_faild");
	}
	sock->setRecvTimerout(time_out);
	int rt = conn->sendRequest(rsq);
	SYLAR_LOG_WARN(g_logger) << "request" << rsq->toString();

	if(rt == 0){
		return std::make_shared<HttpResult>(HttpResult::Error::SEND_CLOSE_BY_PEER,nullptr,
				"sendclose by peer");
		
	}
	if(rt < 0){
		return std::make_shared<HttpResult>(HttpResult::Error::SEND_SOCK_ERROR,nullptr,
				"send sock error errno = " + std::to_string(errno) +" errstr :" + std::string(strerror(errno)));
	}
	HttpResponse::ptr rsp = conn->rcvResponse();
	if(!rsp){
		return std::make_shared<HttpResult>(HttpResult::Error::RCVRESPONSE_TIMEOUT,nullptr,
				"rcv time_out----");
	}
	return std::make_shared<HttpResult>(HttpResult::Error::OK,rsp,"ok");
}






}


}
