#include"fd_manager.h"
#include"hook.h"
#include <asm-generic/socket.h>
#include <unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

namespace sylar{
FdManager::FdManager(){
	m_datas.resize(64);
}
FdCtx::FdCtx(int fd)
	     :m_isInit(false)
	     ,m_isSocket(false)
	     ,m_sysNonblock(false)
	     ,m_UserNonblock(false)
	     ,m_fd(fd)
	     ,m_recvTimeout(0)
	     ,m_sendTimeout(0){
	init();
}

bool FdCtx::init(){
	if(m_isInit){
		return true;
	}	
	struct stat fd_stat;

	if(-1 == fstat(m_fd,&fd_stat)){
		m_isInit = false;
		m_isSocket =false;
	}else{
		m_isInit =  true;
		m_isSocket = S_ISSOCK(fd_stat.st_mode);
	}

	if(m_isSocket){
		int flags = fcntl_f(m_fd,F_GETFL,0);
		if(!(flags & O_NONBLOCK)){
			fcntl_f(m_fd,F_SETFL,flags | O_NONBLOCK);
		}
		m_sysNonblock = true;
	}else{
		m_sysNonblock = false;
	}

	m_UserNonblock = false;
	m_isClosed = false;
	return m_isInit;
}

FdCtx::~FdCtx(){

}
	


FdCtx::ptr FdManager::get(int fd,bool auto_create){
	if(fd <	0){
		return nullptr;
	}
	MutexType::ReadLock lock(m_mutex);
	if(m_datas.size() <= (size_t)fd && !auto_create){
		return nullptr;
	}else{
		if(m_datas[fd]){
			lock.unlock();
			return m_datas[fd];
		}

		lock.unlock();

		MutexType::WriteLock lock2(m_mutex);
		FdCtx::ptr fdctx(new FdCtx(fd));
		m_datas[fd] = fdctx;
		return m_datas[fd];
	}

}
void FdManager::del(int fd){
	MutexType::WriteLock lock(m_mutex);
	if(m_datas.size() <= (size_t)fd){
		return;
	}
	m_datas[fd].reset();
}

bool FdCtx::close(){
	::close(m_fd);
	m_isClosed = true;
	return true;
}
void FdCtx::setTimeout(int type ,uint64_t v){
	if(type == SO_RCVTIMEO){
		m_recvTimeout = v; 
	}else{
		m_sendTimeout = v;
	}

}
uint64_t FdCtx::getTimeout(int type){
	if(type == SO_RCVTIMEO){
		return m_recvTimeout;
	}else{
		return m_sendTimeout;
	}

}



}


