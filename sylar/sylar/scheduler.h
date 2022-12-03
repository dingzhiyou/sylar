#ifndef __SYLAR_SCHEDULER_H__
#define __SYLAR_SCHEDULER_H__

#include <functional>
#include<memory>
#include"fiber.h"
#include "thread.h"
#include<vector>
#include<list>
namespace sylar{

class Scheduler{
public:
	typedef std::shared_ptr<Scheduler> ptr;
	typedef Mutex  MutexType;

	Scheduler(size_t threads = 1,bool use_caller = true,const std::string& name = "");
	virtual ~Scheduler();

	const std::string getName() const {return m_name;}
	static Fiber* GetMainFiber();
	static Scheduler* GetThis();


	void start();
	void stop();

	template<class FiberOrCB>
	void scheduler(FiberOrCB fc ,int thr = -1){
		MutexType::Lock lock(m_mutex);
		bool seed_tickle = schedulerNoLock(fc,thr);
		if(seed_tickle){
			tickle();
		}
	}
	template<class InputIterator>
	void scheduler(InputIterator begin, InputIterator end){
		bool need_tickle = false;
		MutexType::Lock lock(m_mutex);
		while(begin != end){
			need_tickle = schedulerNoLock(&(*begin))  || need_tickle;
			++begin;
		}
		if(need_tickle){
			tickle();
		}
	}

private:
	template<class FiberOrCB>
	bool schedulerNoLock(FiberOrCB fc,int thr = -1){
		bool seed_tickle = m_fibers.empty();
		FiberAndThread  ft(fc,thr);
		if(ft.fiber || ft.cb){
			m_fibers.push_back(ft);
		}	

		return seed_tickle;
	}

private:


	struct FiberAndThread{
		Fiber::ptr fiber;
		std::function<void()> cb;
		int thread;

		FiberAndThread(Fiber::ptr f,int thr):
				fiber(f)
				,thread(thr){}

		FiberAndThread(Fiber::ptr* f,int thr):
			thread(thr){
				fiber.swap(*f);
			}

		FiberAndThread(std::function<void()> c,int thr):
				cb(c)
				,thread(thr)
				{}
		FiberAndThread(std::function<void()>* c,int thr):
				thread(thr){
					cb.swap(*c);
				}

		FiberAndThread():thread(-1){
			
		}

		void reset(){
			fiber = nullptr;
			cb = nullptr;
			thread = -1;
		}
		
		
	};

public:
	size_t getFuncSize(){
		return m_fibers.size();
	}

protected:
        void run();
	virtual void tickle();
	virtual bool stopping();
	virtual void  idle();
	void setThis();
	bool hasIdleThreads(){return m_idlerThreadCount > 0;}

private:
	MutexType m_mutex;
	std::vector<Thread::ptr> m_threads;
	std::string m_name;
	std::list<FiberAndThread> m_fibers;
	Fiber::ptr m_rootFiber;

protected:
	std::vector<int> m_threadIds;
	size_t m_threadCount = 0;
	size_t m_activeThreadCount = 0;
	size_t m_idlerThreadCount = 0;
	bool m_stopping = true;
	bool m_autoStop = false;
	int m_rootThread = 0;
};


}





#endif
