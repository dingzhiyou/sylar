#include "resourceServlet.h"
#include "../sylar/log.h"
#include "http.h"
#include <fstream>
#include <sstream>
#include <string>



namespace sylar{
namespace http{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("root");
uint32_t ResourceServlet::handle(http::HttpRequest::ptr request,http::HttpResponse::ptr response,
				http::HttpSession::ptr session){

	std::string file_path = m_file + request->getPath();

	SYLAR_LOG_DEBUG(g_logger) << "file_path : "<<file_path;
	if(file_path.find("..") != std::string::npos){
		response->setBody("Invalid file_path");
		response->setStatus(HttpStatus::NOT_FOUND);
		return 0;
	}
	std::ifstream ifs;
	ifs.open(file_path);
	if(!ifs){
		response->setBody("file not found");
		response->setStatus(HttpStatus::NOT_FOUND);
		return 0;
	}

	std::string body;
	std::stringstream ss;
	std::string line;
	while(std::getline(ifs,line)){
		ss<<line<<std::endl;
	}
	response->setBody(ss.str());
	response->setHeader("Content-type","html");
	return 0;
}



}

}
