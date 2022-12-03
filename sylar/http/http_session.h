#ifndef __SYLAR_HTTP_SESSION_H__
#define __SYLAR_HTTP_SESSION_H__
#include "../sylar/sockstream.h"
#include "http.h"

#include <boost/type.hpp>
#include <memory>
namespace sylar{
namespace http{
class HttpSession : public SockStream{

public:
	typedef std::shared_ptr<HttpSession> ptr;
	HttpSession(Socket::ptr,bool own = false);

	HttpRequest::ptr rcvRequest();
	int sendResponse(HttpResponse::ptr rsq);	

};


}
}

#endif
