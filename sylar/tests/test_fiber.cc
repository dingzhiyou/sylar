#include "../sylar/sylar.h"
#include "../sylar/fiber.h"
#include <algorithm>
#include <string>
#include<vector>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

void run_fiber(){
	SYLAR_LOG_INFO(g_logger) << "run in fiber";
	sylar::Fiber::GetThis()->swapOut();
	SYLAR_LOG_INFO(g_logger) << "run_end";

}

void test_fiber(){
	sylar::Fiber::GetThis();
	SYLAR_LOG_INFO(g_logger) << "begin";
	sylar::Fiber::ptr fiber(new sylar::Fiber(run_fiber));	
	fiber->swapIn();

	SYLAR_LOG_INFO(g_logger) << "after swap in";
	fiber->swapIn();

	SYLAR_LOG_INFO(g_logger) << "main end";
}

int main(){
	
	std::vector<sylar::Thread::ptr> vec;

	for(int i =0 ;i < 3;i++){
		vec.push_back(sylar::Thread::ptr(new sylar::Thread(test_fiber,"name_" + std::to_string(i))));
	}

	for(int i = 0;i <3;i++){
		vec[i]->join();
	}




}
