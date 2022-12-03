#include"../sylar/sylar.h"
#include "../sylar/uri.h"
#include<iostream>

void run(){

	sylar::Uri::ptr uri = sylar::Uri::Create("http://0.0.0.0:20/xxx?a=2&b=3#frgdd");
	std::cout<<uri->toString()<<std::endl;
	std::cout<<uri->createAddress()->toString()<<std::endl;

}

int main(){
	sylar::IOManager iom(2);
	iom.scheduler(run);
}
