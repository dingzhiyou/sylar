#ifndef __SYLAR_RESOURCESERVLET_H__
#define __SYLAR_RESOURCESERVLET_H__

#include "servlet.h"
#include<memory>

namespace sylar{
namespace http{

class ResourceServlet : public Servlet{
public:
	typedef std::shared_ptr<ResourceServlet> ptr;
	ResourceServlet(const std::string& path):Servlet("ResourceServlet"),m_file(path){}
	uint32_t handle(http::HttpRequest::ptr request,http::HttpResponse::ptr response,
				http::HttpSession::ptr session) override;

private:
	std::string m_file;
};

}
}




#endif
