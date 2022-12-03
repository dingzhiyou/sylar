#include "iomanager.h"
#include "fiber.h"
#include "scheduler.h"
#include"macro.h"
#include"log.h"
#include "thread.h"
#include <asm-generic/errno-base.h>
#include <bits/stdint-uintn.h>
#include <cstring>
#include <functional>
#include <memory>
#include <sys/epoll.h>
#include <unistd.h>
#include<fcntl.h>
namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
IOManager::IOManager(size_t threads ,bool use_caller ,const std::string& name)
		    :Scheduler(threads,use_caller,name){
			    m_epfd = epoll_create(5000);
			    SYLAR_ASSERT(m_epfd > 0);

			    int rt = pipe(m_tikleFds);
			    SYLAR_ASSERT(rt == 0);

			    epoll_event event;
			    memset(&event, 0, sizeof(event));
			    event.events = EPOLLIN | EPOLLET;
			    event.data.fd = m_tikleFds[0];
			    rt = fcntl(m_tikleFds[0],F_SETFL,O_NONBLOCK);
			    SYLAR_ASSERT(rt == 0);
			    rt = epoll_ctl(m_epfd,EPOLL_CTL_ADD, m_tikleFds[0], &event);
			    ++m_pendingEventCount;
			    SYLAR_ASSERT(rt == 0);
			    //m_fdContexts.resize(64);
			    start();

}

void IOManager::onTimerInsertedAtFront(){
	tickle();

}

void IOManager::contextResize(size_t size){
	m_fdContexts.resize(size);
	for(size_t i = 0;i < m_fdContexts.size();i++){
		if(!m_fdContexts[i]){
			m_fdContexts[i] = new FdContext;
			m_fdContexts[i]->fd = i;
		}
	}
}
IOManager::~IOManager(){
	stop();
	close(m_epfd);
	close(m_tikleFds[0]);
	close(m_tikleFds[1]);

	for(size_t i = 0;i < m_fdContexts.size();i++){
		if(m_fdContexts[i]){
		delete m_fdContexts[i];
		}
	}

}

int IOManager::addEvent(int fd,Event event,std::function<void()> cb){
	FdContext* fd_ctx = nullptr;
	RWMutex::ReadLock lock(m_mutex);
	if(m_fdContexts.size() > (size_t)fd){
		fd_ctx = m_fdContexts[fd];
		lock.unlock();
	}else{
		lock.unlock();
		RWMutex::WriteLock lock2(m_mutex);
		contextResize(fd * 1.5);
		fd_ctx = m_fdContexts[fd];
	}
	FdContext::MutexType::Lock lock2(fd_ctx->mutex);
	
	
	
	if(fd_ctx->events & event){
		SYLAR_LOG_ERROR(g_logger) << "event add error" << " fd = "<<fd;
		SYLAR_ASSERT2(false, "event add error");
	}
	
	int op  = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
	epoll_event epevent;
	epevent.events = EPOLLET | fd_ctx->events | event;
	epevent.data.ptr = fd_ctx;

	int rt = epoll_ctl(m_epfd,op,fd,&epevent);
	if(rt){
		SYLAR_LOG_ERROR(g_logger) << "epoll_ctl"<<m_epfd << ", "
					  << op<<","<< fd <<","<<errno<<":"<<strerror(errno);
	}

	++m_pendingEventCount;
	fd_ctx->events = (Event)(fd_ctx->events | event);
	FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
	SYLAR_ASSERT(!event_ctx.scheduler);
	SYLAR_ASSERT(!event_ctx.fiber);
	SYLAR_ASSERT(!event_ctx.cb);



	//Fix Me
	event_ctx.scheduler = Scheduler::GetThis();
	if(cb){
		SYLAR_LOG_INFO(g_logger) << "add cb successful";
		event_ctx.cb.swap(cb);
	}else{
		event_ctx.fiber = Fiber::GetThis();
		SYLAR_ASSERT(event_ctx.fiber->getState() == Fiber::EXEC);
	}

	return 0;
}
bool IOManager::delEvent(int fd,Event event){
	MutexType::ReadLock lock(m_mutex);
	if((size_t)fd >= m_fdContexts.size()){
		return false;
	}

	FdContext* fd_ctx = m_fdContexts[fd];
	lock.unlock();

	FdContext::MutexType::Lock lock1(fd_ctx->mutex);
	if(!(fd_ctx->events & event)){
		return false;
	}

	Event new_events = (Event)(fd_ctx->events & ~event);
	int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;

	epoll_event epevent;
	epevent.events = EPOLLET | new_events;
	epevent.data.ptr = fd_ctx;

	int rt = epoll_ctl(m_epfd,op,fd,&epevent);
	if(rt){
		SYLAR_LOG_ERROR(g_logger) << "epoll_ctl"<<m_epfd << ", "
					  << op<<","<< fd <<","<<errno<<":"<<strerror(errno);
		return false;
	}

	--m_pendingEventCount;
	fd_ctx->events = new_events;
	FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
	fd_ctx->resetContext(event_ctx);
	return true;
	

}
IOManager::FdContext::EventContext&  IOManager::FdContext::getContext(IOManager::Event event){
	switch (event) {
		case IOManager::READ :
			return read;
		case IOManager::WRITE :
			return write;
			default:
			SYLAR_ASSERT2(false,"getContext");
	}
}
void IOManager::FdContext::resetContext(FdContext::EventContext& ctx){
	ctx.scheduler = nullptr;
	ctx.fiber.reset();
	ctx.cb = nullptr;
}
bool IOManager::cancelEvent(int fd,Event event){
	MutexType::ReadLock lock(m_mutex);
	if((size_t)fd >= m_fdContexts.size()){
		return false;
	}

	FdContext* fd_ctx = m_fdContexts[fd];
	lock.unlock();

	FdContext::MutexType::Lock lock1(fd_ctx->mutex);
	if(!(fd_ctx->events & event)){
		return false;
	}

	Event new_events = (Event)(fd_ctx->events & ~event);
	int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;

	epoll_event epevent;
	epevent.events = EPOLLET | new_events;
	epevent.data.ptr = fd_ctx;

	int rt = epoll_ctl(m_epfd,op,fd,&epevent);
	if(rt){
		SYLAR_LOG_ERROR(g_logger) << "epoll_ctl"<<m_epfd << ", "
					  << op<<","<< fd <<","<<errno<<":"<<strerror(errno);
		return false;
	}

	fd_ctx->triggerEvent(event);
	--m_pendingEventCount;
	return true;
	
}

bool IOManager::cancleAll(int fd){
	MutexType::ReadLock lock(m_mutex);
	if((size_t)fd >= m_fdContexts.size()){
		return false;
	}

	FdContext* fd_ctx = m_fdContexts[fd];
	lock.unlock();

	FdContext::MutexType::Lock lock1(fd_ctx->mutex);
	if(!fd_ctx->events){
		return false;
	}

	int op = EPOLL_CTL_DEL;

	epoll_event epevent;
	epevent.events = 0;
	epevent.data.ptr = fd_ctx;

	int rt = epoll_ctl(m_epfd,op,fd,&epevent);
	if(rt){
		SYLAR_LOG_ERROR(g_logger) << "epoll_ctl"<<m_epfd << ", "
					  << op<<","<< fd <<","<<errno<<":"<<strerror(errno);
		return false;
	}

	if(fd_ctx->events & READ){
		fd_ctx->triggerEvent(READ);
		--m_pendingEventCount;	
	}
	if(fd_ctx->events & WRITE){
		fd_ctx->triggerEvent(WRITE);
		--m_pendingEventCount;	
	}
	SYLAR_ASSERT(fd_ctx->events == 0);
	return 0;
}
void IOManager::FdContext::triggerEvent(IOManager::Event event){
	SYLAR_ASSERT(events & event);
	events = (Event)(events & ~event);

	EventContext& ctx = getContext(event);

	if(ctx.cb){
		ctx.scheduler->scheduler(&ctx.cb);
	}else{
		ctx.scheduler->scheduler(&ctx.fiber);
	}	

	ctx.scheduler = nullptr;
	return;
}


IOManager* IOManager::GetThis(){
	return dynamic_cast<IOManager*>(Scheduler::GetThis());
}

void IOManager::tickle() {
	if(hasIdleThreads()){
		return;
	}
	int rt = write(m_tikleFds[1],"1",1);
	SYLAR_ASSERT(rt == 1);
}
void IOManager::idle() {
	epoll_event* events = new epoll_event[64];
	std::shared_ptr<epoll_event> shared_events(events,[](epoll_event* ptr){
				delete[] ptr;
			});
	int rt;
	uint64_t next_timeout = 0;
	while(true){
		next_timeout = getNextTimer();
		if(stopping(next_timeout)){
			if(next_timeout == ~0ull){
				SYLAR_LOG_INFO(g_logger) << " idle stopping exit";
		       		break;
			}
		}
		do{
			static const int MAX_TIMEOUT = 5000;
			int real_timeout = MAX_TIMEOUT > (int)next_timeout ?  next_timeout : MAX_TIMEOUT;

		        rt = epoll_wait(m_epfd,events,64,real_timeout);
			if(rt < 0 && errno == EINTR){
			}else{
				break;
			}
		}while(true);
		std::vector<std::function<void()> > cbs;
		listExpiredCb(cbs);

		for(size_t i = 0;i < cbs.size();i++){
			scheduler(cbs[i]);
		}
		//if(cbs.size() > 0){
		//	scheduler(cbs[0]);
		//	SYLAR_LOG_INFO(g_logger) << "fiber size:  "<< Scheduler::GetThis()->getFuncSize();
		//}
		cbs.clear();

		for(int i = 0;i < rt;i++){
			epoll_event& event = events[i];
			if(event.data.fd == m_tikleFds[0]){
				uint8_t dummy;
				while(read(m_tikleFds[1],&dummy,1) == 1);
				continue;
			}
			FdContext* fd_ctx = (FdContext*)event.data.ptr;
			FdContext::MutexType::Lock lock(fd_ctx->mutex);
			if(event.events & (EPOLLERR | EPOLLHUP)){
				event.events |= EPOLLIN | EPOLLOUT;
			}
			int real_events = NONE;
			if(event.events & EPOLLIN){
				real_events |= READ;
			}
			if(event.events & EPOLLOUT){
				real_events |= WRITE;
			}

			if((fd_ctx->events & real_events) == NONE){
				continue;
			}

			if( (real_events & WRITE) && (fd_ctx->events & WRITE) ){
				fd_ctx->triggerEvent(WRITE);
				--m_pendingEventCount;
			}
			if((real_events & READ) && (fd_ctx->events & READ)){
				fd_ctx->triggerEvent(READ);
				--m_pendingEventCount;
			}

			int left_event = (fd_ctx->events & ~real_events);
			int op = left_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
			event.events = EPOLLET | left_event;

			int rt2 = epoll_ctl(m_epfd,op,fd_ctx->fd,&event);
			if(rt2){
				SYLAR_LOG_INFO(g_logger) << " epoll_ctl error";
				continue;
			}
			
			
		}
	
		Fiber::ptr cur = Fiber::GetThis();
		auto raw_ptr = cur.get();
		cur.reset();
		raw_ptr->swapOut();

	}

	
}

bool IOManager::stopping(uint64_t& timeout){
	timeout = getNextTimer();
	return timeout == ~0ull 
	       &&m_pendingEventCount == 0
       	       && Scheduler::stopping();	       

}
bool IOManager::stopping() {
	uint64_t timeout = 0;
	return  stopping(timeout);
}



}
