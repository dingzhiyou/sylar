#ifndef __SYLAR_TIMER_H__
#define __SYLAR_TIMER_H__
#include<set>
#include"thread.h"
#include <bits/stdint-uintn.h>
#include <functional>
#include<memory>
#include <vector>

namespace sylar{

class TimerManager;
class Timer : public std::enable_shared_from_this<Timer>{
friend class TimerManager;
public:
	typedef std::shared_ptr<Timer> ptr;
	struct Comparator{
		bool operator()(const Timer::ptr& lhs,const Timer::ptr& rhs);
	};

	bool cancle();
	bool reset(uint64_t ms,bool from_now = true);
	bool refresh();



private:
	Timer(uint64_t ms,std::function<void()> cb, bool recurring,TimerManager* manager);
private:
	uint64_t m_ms = 0; //执行周期
	std::function<void()> m_cb;
	bool m_recurring = false;
	TimerManager* m_manager = nullptr;
	uint64_t m_next = 0;//精确的执行时间
private:
};
class TimerManager{
friend class Timer;
public:
	typedef RWMutex MutexType;
	TimerManager();
	virtual ~TimerManager();
	Timer::ptr addTimer(uint64_t ms,std::function<void()> cb,bool recurring = false);
	Timer::ptr addCondtionTimer(uint64_t ms,std::function<void()> cb,std::weak_ptr<void> weak_cond,bool recurring = false);
	uint64_t getNextTimer();
	void listExpiredCb(std::vector<std::function<void()> >& cbs );

protected:
	virtual void onTimerInsertedAtFront() = 0;
	void addTimer(Timer::ptr& timer);
	bool hasTimer();
private:
private:
	MutexType m_mutex;
	std::set<Timer::ptr,Timer::Comparator> m_timers;
};

}



#endif
