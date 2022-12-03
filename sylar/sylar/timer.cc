#include "timer.h"
#include "thread.h"
#include "util.h"
#include <functional>

namespace sylar{

void TimerManager::addTimer(Timer::ptr& timer){
	MutexType::WriteLock lock(m_mutex);
	auto it = m_timers.insert(timer).first;
	lock.unlock();
	bool at_front = (it == m_timers.begin());
	if(at_front){
		onTimerInsertedAtFront();
	}
}
bool TimerManager::hasTimer(){
	if(getNextTimer() != ~0ull){
		return true;
	}
	return false;
}
Timer::Timer(uint64_t ms,std::function<void()> cb, bool recurring,TimerManager* manager): 
	    m_ms(ms)
	    ,m_cb(cb)
	    ,m_recurring(recurring)
	    ,m_manager(manager){
  	    m_next = GetCurrentMs() + m_ms;	
	
}

bool Timer::Comparator::operator()(const Timer::ptr& lhs,const Timer::ptr& rhs){
	  if(!lhs  && !rhs){
	  	return false;
	  }

	  if(!lhs){
	  	return true;
	  }
	  if(!rhs){
	  	return false;
	  }

	  if(lhs->m_next < rhs->m_next){
	  	return true;
	  }else{
	  	return false;
	  }
}

bool Timer::cancle(){
	TimerManager::MutexType::WriteLock lock(m_manager->m_mutex);
	if(m_cb){
		m_cb = nullptr;
		auto it = m_manager->m_timers.find(shared_from_this());
		m_manager->m_timers.erase(it);
		return true;
	}

	return false;
}

bool Timer::reset(uint64_t ms,bool from_now){
	TimerManager::MutexType::WriteLock lock(m_manager->m_mutex);
	if(!m_cb){
		return false;
	}
	auto it = m_manager->m_timers.find(shared_from_this());
	if(it == m_manager->m_timers.end()){
		return false;
	}
	m_manager->m_timers.erase(it);
	m_ms = ms;
	m_next = GetCurrentMs() + m_ms;
	auto self = shared_from_this();
	m_manager->addTimer(self);
	return true;



}
bool Timer::refresh(){
	TimerManager::MutexType::WriteLock lock(m_manager->m_mutex);
	if(!m_cb){
		return false;
	}
	auto it = m_manager->m_timers.find(shared_from_this());
	if(it == m_manager->m_timers.end()){
		return false;
	}
	m_manager->m_timers.erase(it);
	m_next = GetCurrentMs() + m_ms;
	m_manager->m_timers.insert(shared_from_this());
	return true;
}

void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs ){
	uint64_t now_ms = sylar::GetCurrentMs();
	std::vector<Timer::ptr> expired;
	MutexType::WriteLock lock(m_mutex);
	for(auto it = m_timers.begin(); it != m_timers.end();){
		if((*it)->m_next < now_ms){
			cbs.push_back((*it)->m_cb);
			expired.push_back(*it);
			m_timers.erase(it++);
		}
		break;
	}



	for(auto& timer : expired){
		if(timer->m_recurring){
			timer->m_next = now_ms + timer->m_ms;
			m_timers.insert(timer);
		}else{
			timer->m_cb = nullptr;
		}
	}

}
uint64_t TimerManager::getNextTimer(){
	MutexType::ReadLock lock(m_mutex);
	if(m_timers.empty()){
		return ~0ull;
	}
	const Timer::ptr& next = *m_timers.begin();
	uint64_t now_ms = sylar::GetCurrentMs();
	if(now_ms >= next->m_next){
		return 0;
	}else{
		return next->m_next - now_ms;
	}


}

TimerManager::TimerManager(){

}
TimerManager::~TimerManager(){

}
Timer::ptr TimerManager::addTimer(uint64_t ms,std::function<void()> cb,bool recurring ){
	Timer::ptr timer(new Timer(ms,cb,recurring,this));
	MutexType::WriteLock lock(m_mutex);
	auto it = m_timers.insert(timer).first;
	
	bool at_front = (it == m_timers.begin());
	lock.unlock();

	if(at_front){
		onTimerInsertedAtFront();
	}
	return timer;
}

static void OnTimer(std::weak_ptr<void> weak_cond,std::function<void()> cb){
	std::shared_ptr<void> tmp = weak_cond.lock();
	if(tmp){
		cb();
	}
}

Timer::ptr TimerManager::addCondtionTimer(uint64_t ms,std::function<void()> cb,std::weak_ptr<void> weak_cond,bool recurring){
		return addTimer(ms,std::bind(&OnTimer,weak_cond,cb),recurring);

}

}
