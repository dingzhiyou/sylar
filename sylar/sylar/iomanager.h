#ifndef __SYLAR__IOMANAGER_H__
#define __SYLAR__IOMANAGER_H__

#include"scheduler.h"
#include "thread.h"
#include"timer.h"
#include <atomic>
#include <functional>
#include <memory>
#include <vector>


namespace sylar{

class IOManager: public Scheduler , public TimerManager{
public:
	typedef std::shared_ptr<IOManager> ptr;
	typedef RWMutex MutexType;

	enum Event{
		NONE = 0x0,
		READ = 0x1,
		WRITE = 0x4,
	};


private:
	struct FdContext{
		typedef Mutex MutexType;
		struct EventContext{
			Scheduler* scheduler = nullptr;
			Fiber::ptr fiber;
			std::function<void()> cb;
		};
		EventContext& getContext(Event event);
		void resetContext(EventContext& ctx);
		void triggerEvent(Event event);
		int fd = 0;
		EventContext read;
		EventContext write;
		Event events = Event::NONE;
		Mutex mutex;
	};

public:
	IOManager(size_t threads = -1,bool use_caller = true,const std::string& name = "");
	~IOManager();
	
	size_t  getEventCount()const {return m_pendingEventCount.load();}
	int addEvent(int fd,Event event,std::function<void()> cb = nullptr);
	bool delEvent(int fd,Event event);
	bool cancelEvent(int fd,Event event);

	bool cancleAll(int fd);

	bool stopping(uint64_t& timeout);
	static IOManager* GetThis();
protected:

	void tickle() override;
	void idle() override;
	bool stopping() override;
	void contextResize(size_t size);
	void onTimerInsertedAtFront() override;
private:
	int m_epfd = 0;
	int m_tikleFds[2];

	std::atomic<size_t> m_pendingEventCount = {0};
	RWMutex m_mutex;
	std::vector<FdContext*> m_fdContexts;

};


}



#endif
