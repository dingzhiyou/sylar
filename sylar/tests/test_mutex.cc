#include "../sylar/sylar.h"

int main(){
	
	sylar::Mutex mutex;
	sylar::Mutex::Lock lock(mutex);
	sylar::Mutex::Lock lock1(mutex);

}
