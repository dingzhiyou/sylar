#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include <bits/stdint-uintn.h>
#include <functional>
#include<memory>
#include "thread.h"
#include <sys/ucontext.h>
#include <ucontext.h>
namespace sylar{

class Fiber : public std::enable_shared_from_this<Fiber>{
friend class Scheduler;
public:
	typedef std::shared_ptr<Fiber> ptr;
	enum State{
		INIT,
		HOLD,
		EXEC,
		TERM,
		READY,
		EXCEPT
	};
private:
	Fiber();

public:
	Fiber(std::function<void()> cb,size_t stacksize = 0,bool use_caller = false);
	~Fiber();
	void call();
	void back();
	void reset(std::function<void()> cb);
	void swapIn();
	void swapOut();
	uint64_t getId() const {return m_id;}
	void setState(State s){m_state = s;}
	State getState()const {return m_state;}
public:
	static void SetThis(Fiber* f);
	Fiber* GetThis(Fiber* f);
	static Fiber::ptr GetThis();
	static void YieldToReady();
	static void YieldToHold();

	static uint64_t TotalFibers();
	static uint64_t GetFiberId();
	static void MainFunc();
	static void CallerMainFunc();

private:
	uint64_t m_id;
	uint32_t m_stacksize;
	State m_state = INIT;

	ucontext_t m_ctx;
	void* m_stack = nullptr;

	std::function<void()> m_cb;
};

}

#endif
