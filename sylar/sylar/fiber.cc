#include "fiber.h"
#include "log.h"
#include "macro.h"
#include "scheduler.h"
#include"util.h"
#include<atomic>
#include <bits/posix_opt.h>
#include <cstdlib>
#include <exception>
#include <sys/types.h>
#include<thread>
#include <memory>
#include <ucontext.h>

#define STACKSIZE 1024 * 1024
namespace sylar{
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

static thread_local Fiber* t_fiber = nullptr;
static thread_local std::shared_ptr<Fiber> t_threadFiber = nullptr;

class MallocStackAllocator{
public:	
	static void* Alloc(uint32_t size){
		return malloc(size);
	}	

	static void Dealloc(void* vp,size_t size){
		free(vp);
	}


		
};
using StackAllocator = MallocStackAllocator;	
Fiber::Fiber(){
	m_id = s_fiber_id;
	m_state = EXEC;
	SetThis(this);
	if(getcontext(&m_ctx)){
		SYLAR_ASSERT2(false, "getcontext error");
	}
	++s_fiber_count;
}
void Fiber::SetThis(Fiber *f){
	t_fiber = f;
}
Fiber::ptr Fiber::GetThis(){
	if(t_fiber){
		return t_fiber->shared_from_this();
	}
	Fiber::ptr main_fiber(new Fiber);
	SYLAR_ASSERT(t_fiber == main_fiber.get());
	t_threadFiber = main_fiber;
	return t_fiber->shared_from_this();
}

Fiber::Fiber(std::function<void()> cb,size_t stacksize,bool use_caller):m_id(++s_fiber_id)
							,m_cb(cb){

	SYLAR_LOG_DEBUG(SYLAR_LOG_ROOT()) << "Fiber id: "<< s_fiber_id; 
	++s_fiber_count;
	m_stacksize = STACKSIZE;
	m_stack = StackAllocator::Alloc(m_stacksize);
	if(getcontext(&m_ctx)){
		SYLAR_ASSERT2(false, "getcontext error");
	}

	m_ctx.uc_link = nullptr;
	m_ctx.uc_stack.ss_sp = m_stack;
	m_ctx.uc_stack.ss_size = m_stacksize;

	if(!use_caller){
		makecontext(&m_ctx,&Fiber::MainFunc,0);
	}else{
		makecontext(&m_ctx,&Fiber::CallerMainFunc,0);
	}

}

void Fiber::CallerMainFunc(){
	Fiber::ptr cur = GetThis();
	SYLAR_ASSERT2(cur,"error");
	try{
		cur->m_cb();
		cur->m_cb = nullptr;
		cur->m_state = TERM;
	}catch(std::exception& ex){
		cur->m_state = EXCEPT;
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Fiber Exception "<<ex.what()
						  <<std::endl
						  <<BacktraceToString(100, 2);
	}catch(...){
		cur->m_state = EXCEPT;
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Fiber Exception ";
	}

	auto raw_ptr = cur.get();
	cur.reset();
	raw_ptr->back();
	SYLAR_ASSERT2(false ,"never reached");
}
void Fiber::call(){
	SetThis(this);
	m_state = EXEC;
	if(swapcontext(&(t_threadFiber)->m_ctx, &m_ctx)){
		SYLAR_ASSERT2(false, "swapcontext");
	}

}

void Fiber::back(){
		SetThis(t_threadFiber.get());
		if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)){
			SYLAR_ASSERT2(false, "swapcontext error");
		}
}
Fiber::~Fiber(){
	SYLAR_LOG_DEBUG(SYLAR_LOG_ROOT()) <<  "~Fiber id = "<< getId() ;  
	--s_fiber_count;
	if(m_stack){
		SYLAR_ASSERT(m_state == TERM || m_state == INIT) ;
		StackAllocator::Dealloc(m_stack,m_stacksize);
	}else{
	
		SYLAR_ASSERT(!m_cb);
		SYLAR_ASSERT(m_state == EXEC);
		Fiber* cur = t_fiber;
		if(cur == this){
			SetThis(nullptr);
		}
	}

}
  
void Fiber::reset(std::function<void()> cb){
	SYLAR_ASSERT(m_stack);
	SYLAR_ASSERT(m_state == TERM || m_state == EXCEPT
			|| m_state == INIT);
	m_cb = cb;
	if(getcontext(&m_ctx)){
		SYLAR_ASSERT2(false,"getcontex() error");
	}
	m_ctx.uc_link = nullptr;
	m_ctx.uc_stack.ss_sp = m_stack;
	m_ctx.uc_stack.ss_size = m_stacksize;

	makecontext(&m_ctx,&Fiber::MainFunc,0);
	m_state = INIT;
}
void Fiber::swapIn(){
	SetThis(this);
	//SYLAR_ASSERT(m_state != EXEC);
	m_state = EXEC;
	if(swapcontext(&Scheduler::GetMainFiber()->m_ctx , &m_ctx)){
		SYLAR_ASSERT2(false, "swapcontext error");
	}


}

uint64_t Fiber::GetFiberId(){
	return t_fiber->getId();
}
void Fiber::swapOut(){
		SetThis(Scheduler::GetMainFiber());
		if(swapcontext(&m_ctx,&Scheduler::GetMainFiber()->m_ctx)){
			SYLAR_ASSERT2(false, "swapcontext error");
		}

}
void Fiber::YieldToReady(){
	Fiber::ptr cur = GetThis();
	cur->m_state = READY;
	cur->swapOut();
}
void Fiber::YieldToHold(){
	Fiber::ptr cur = GetThis();
	cur->m_state =HOLD;
	cur->swapOut();
}

uint64_t Fiber::TotalFibers(){
	return s_fiber_count;
}

void Fiber::MainFunc(){
	Fiber::ptr cur = GetThis();
	SYLAR_ASSERT2(cur,"error");
	//try{
		cur->m_cb();
		cur->m_cb = nullptr;
		cur->m_state = TERM;
	//}catch(std::exception& ex){
	//	cur->m_state = EXCEPT;
	//	SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Fiber Exception "<<ex.what()
	//					  <<std::endl
	//					  <<BacktraceToString(100, 2);
	//}catch(...){
	//	cur->m_state = EXCEPT;
	//	SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Fiber Exception ";
	//}

	auto raw_ptr = cur.get();
	raw_ptr->setState(TERM);
	cur.reset();
	raw_ptr->swapOut();

	SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "FibeId " << GetFiberId();
	SYLAR_ASSERT2(false ,"never reached");
}

}
