#include "../sylar/sylar.h"
#include "../sylar/macro.h"
#include <assert.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
void func_in(){
	SYLAR_ASSERT2(1==1,"xxxxx");
	SYLAR_LOG_INFO(g_logger) << sylar::BacktraceToString(10, 2);

}

void func(){
	func_in();
}

void test_assert(){

	func();
}

int main(){
	
	test_assert();;

}
