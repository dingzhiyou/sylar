#include "thread.h"
#include "log.h"
#include "util.h"
#include <functional>
#include <pthread.h>
#include <semaphore.h>
#include <stdexcept>


namespace sylar{
static thread_local Thread* t_thread = nullptr;
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
Thread* Thread::GetThis(){
	return t_thread;
}
std::string Thread::GetName() {
  return  t_thread->getName();
}

void Thread::SetName(const std::string& name){
	t_thread->m_name = name;
}
void* Thread::run(void *arg){
	Thread* thread = (Thread*)arg;
	t_thread = thread;
	thread->m_id = GetThreadId();
	pthread_setname_np(pthread_self(),thread->m_name.substr(0,15).c_str());
	std::function<void()> cb;
	cb.swap(thread->m_cb);
	thread->m_semaphore.notify();
	cb();
	return nullptr;
}

Thread::Thread(std::function<void()> cb,const std::string& name):m_cb(cb),m_name(name){
	int rt = pthread_create(&m_thread,nullptr ,&Thread::run, this);
	if(rt){
		SYLAR_LOG_ERROR(g_logger) << "create thread error";
		throw std::logic_error("pthread_create error");
	}	
	
	m_semaphore.wait();
}
Thread:: ~Thread(){
	if(m_thread){
		pthread_detach(m_thread);
	}
}



void Thread::join(){
	if(m_thread){
 		int rt = pthread_join(m_thread,nullptr);
		if(rt){
			SYLAR_LOG_ERROR(g_logger) << "join thread error";
			throw std::logic_error("pthread_join error");
		}
	}
	m_thread = 0;
}

Semaphore::Semaphore(uint32_t count){
	if(sem_init(&m_semaphore,0 ,count)){
		throw std::logic_error("sem_init error");
	}

}
Semaphore::~Semaphore(){
	sem_destroy(&m_semaphore);
}
void Semaphore::wait(){
	while(true){
		if(!sem_wait(&m_semaphore)){
			return;
		}
	}
}
void Semaphore::notify(){
	if(sem_post(&m_semaphore)){
		throw std::logic_error("sem_post error");
	}
}


}
