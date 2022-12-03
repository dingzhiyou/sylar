#include "../http/http.h"
#include<iostream>
#include <sstream>
int main(){
	sylar::http::HttpRequest req;
	req.setMethod(sylar::http::HttpMethod::GET);
	req.setPath("/home/data");
	std::string key = "Host";
	std::string value = "www.baidu.com";
	req.setHead(key, value);

	req.setHead("aaaaa", "ccccc");
	req.setBody("kkkkkkkkkk");
	std::stringstream ss;
	req.dump(ss);
	std::cout<<ss.str()<<std::endl;
	std::cout<<std::endl;
	std::cout<<std::endl;
	std::cout<<std::endl;


	sylar::http::HttpResponse res;
	res.setStatus(sylar::http::HttpStatus::OK);
	std::string key1 = "Host";
	std::string value1 = "www.baidu.com";
	res.setHeader(key1, value1);
	res.setBody("kkkkkkkkkk");
	std::stringstream ss1;
	res.dump(ss1);
	std::cout<<ss1.str()<<std::endl;

}
