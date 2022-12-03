#ifndef __SYLAR_THREAD_H__
#define __SYLAR_THREAD_H__

#include <memory>
#include<thread>
#include<functional>
#include<pthread.h>
#include<semaphore.h>
#include <stdint.h>
#include "noncopyable.h"

namespace sylar{

class Semaphore : public Noncopyable{
	
public:
	typedef std::shared_ptr<Semaphore> ptr;
	Semaphore(uint32_t count = 0);
	void wait();
	void notify();
	~Semaphore();

private:
	Semaphore(const Semaphore&) = delete;
	Semaphore(const Semaphore&& ) = delete;
	Semaphore& operator=(const Semaphore&) = delete;
private:
	sem_t m_semaphore;
};

template<class T>
struct ScopeLockImpl{
public:
	ScopeLockImpl(T & mutex):m_mutex(mutex){
		m_mutex.lock();
		m_locked = true;
	}
	~ScopeLockImpl(){
		unlock();
	}
	void lock(){
		if(!m_locked){
			m_mutex.lock();
			m_locked = true;
		}
	}
	void unlock(){
		if(m_locked){
			m_mutex.unlock();
			m_locked = false;
		}
	}
private:
	T& m_mutex;
	bool m_locked = false;

	

};



class Spinlock: public Noncopyable{
public:
	typedef ScopeLockImpl<Spinlock> Lock;
	Spinlock(){
		pthread_spin_init(&m_mutex, 0);
	}
	~Spinlock(){
		pthread_spin_destroy(&m_mutex);
	}
	void lock(){
		pthread_spin_lock(&m_mutex);
	}
	void unlock(){
		pthread_spin_unlock(&m_mutex);
	}

private:
	pthread_spinlock_t m_mutex;
};
class Mutex : public Noncopyable{
public:
	typedef ScopeLockImpl<Mutex> Lock;
	typedef std::shared_ptr<Mutex> ptr;
	Mutex(){
		pthread_mutex_init(&m_mutex,nullptr);
	}
	~Mutex(){
		pthread_mutex_destroy(&m_mutex);
	}

	void lock(){
		pthread_mutex_lock(&m_mutex);
	}

	void unlock(){
		pthread_mutex_unlock(&m_mutex);
	}
private:
	pthread_mutex_t m_mutex;
};
template<class T>
struct ReadScopeLockImpl{
public:
	ReadScopeLockImpl(T & mutex):m_mutex(mutex){
		m_mutex.rdlock();
		m_locked = true;
	}
	~ReadScopeLockImpl(){
		unlock();
	}
	void lock(){
		if(!m_locked){
			m_mutex.rdlock();
			m_locked = true;
		}
	}
	void unlock(){
		if(m_locked){
			m_mutex.unlock();
			m_locked = false;
		}
	}
private:
	T& m_mutex;
	bool m_locked = false;
};
template<class T>
struct WriteScopeLockImpl{
public:
	WriteScopeLockImpl(T & mutex):m_mutex(mutex){
		m_mutex.wrlock();
		m_locked = true;
	}
	~WriteScopeLockImpl(){
		unlock();
	}
	void lock(){
		if(!m_locked){
			m_mutex.wrlock();
			m_locked = true;
		}
	}
	void unlock(){
		if(m_locked){
			m_mutex.unlock();
			m_locked = false;
		}
	}
private:
	T& m_mutex;
	bool m_locked = false;
};
class RWMutex{
public:
	typedef std::shared_ptr<RWMutex> ptr;
	typedef ReadScopeLockImpl<RWMutex>  ReadLock;
	typedef WriteScopeLockImpl<RWMutex> WriteLock;
	RWMutex(){
		pthread_rwlock_init(&m_mutex, nullptr);
	}
	~RWMutex(){
		pthread_rwlock_destroy(&m_mutex);
	}

	void rdlock(){
		pthread_rwlock_rdlock(&m_mutex);
	}
	void wrlock(){
		pthread_rwlock_wrlock(&m_mutex);
	}
	void unlock(){
		pthread_rwlock_unlock(&m_mutex);
	}
private:
	pthread_rwlock_t m_mutex;
};


class Thread: public Noncopyable{
public:
	typedef std::shared_ptr<Thread> ptr;
	Thread(std::function<void()> cb,const std::string& name);
	~Thread();


	std::string getName() { return m_name;}
	pid_t getId() const {return m_id;}

	void join();
	static Thread* GetThis();
	static std::string GetName();
	static void SetName(const std::string& name);
	static void* run(void* arg);
private:
	Thread(const Thread& ) = delete;
	Thread(const Thread&& ) = delete;
	Thread& operator=(const Thread&) = delete;


private:
	pid_t m_id;
	pthread_t m_thread = 0;
	std::function<void()> m_cb;
	std::string m_name;

	Semaphore m_semaphore;
};



}



#endif
