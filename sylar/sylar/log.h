#ifndef __SYLAY_LOG_H__
#define __SYLAY_LOG_H__

#include"singleton.h"
#include <bits/stdint-uintn.h>
#include <fstream>
#include<memory>
#include <cstdint>
#include<map>
#include <ostream>
#include<string>
#include<list>
#include<sstream>
#include<fstream>
#include<vector>
#include "util.h"
#define SYLAR_LOG_LEVEL(logger,level) \
	if(level >= logger->getLevel()) \
	sylar::LogEventWarp(sylar::LogEvent::ptr(new sylar::LogEvent(logger,level,__FILE__,__LINE__,0,sylar::GetThreadId(),sylar::GetFriberId(),\
			time(0)))).getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::DEBUG) 
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::WARN) 
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::INFO) 
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::ERROR) 
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::FATAL) 

#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()  
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstance()->getLogger(name)
namespace sylar{
class Logger;
class LoggerManager;
class LogLevel{
public:
	enum Level{
	    UNKNOW = 0,
	    DEBUG = 1,
	    INFO = 2,
	    WARN = 3,
	    ERROR = 4,
	    FATAL = 5
	};
	static const char* toString(LogLevel::Level level);

};


class LogEvent{
public:
	typedef std::shared_ptr<LogEvent> ptr;
	LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char* file,
		       	uint32_t line, uint32_t elapse , uint32_t threadid, uint32_t friberid
			,uint64_t time);
	
	const char* getFile() const {return m_file;}
	uint32_t getLine()const {return m_line;}
	uint32_t getElapse()const {return m_elapse;}
	uint32_t getThreadId()const {return m_threadId;}
	uint32_t getFriberId() const {return m_fiberId;}
	uint64_t getTime() const {return m_time;}
	bool  getError()const {return m_error;}
	std::string getContent() const {return m_ss.str();}
	std::shared_ptr<Logger> getLogger()const {return m_logger;}
	LogLevel::Level getLevel(){return m_level;}
	std::stringstream& getSS() {return m_ss;}
private:
	const char* m_file = nullptr;
	uint32_t m_line = 0;
	uint32_t m_elapse = 0;
	uint32_t m_threadId = 0;
	uint32_t m_fiberId = 0;
	uint64_t m_time = 0;
	std::shared_ptr<Logger> m_logger;
	LogLevel::Level m_level;
	bool m_error = false;
	std::stringstream m_ss;

};

class LogEventWarp
{
public:
	LogEventWarp(LogEvent::ptr event):m_event(event)
	{
	}

	~LogEventWarp();
	std::stringstream& getSS() {return m_event->getSS();}
private:
		LogEvent::ptr m_event;
};


class LogFormatter{
public: 
	typedef std::shared_ptr<LogFormatter> ptr;
	
	LogFormatter(const std::string& pattern);
	std::string  format(std::shared_ptr<Logger> ptr,LogLevel::Level level, LogEvent::ptr event);
	class FormatItem{
	public:
		typedef  std::shared_ptr<FormatItem> ptr;
		virtual ~FormatItem(){}
		virtual void format(std::shared_ptr<Logger> ptr,std::stringstream& ss,LogLevel::Level level,LogEvent::ptr event) = 0;
	};

	void init();
private:
	std::vector<FormatItem::ptr> m_items;
	std::string m_pattern;
	

};


class LogAppender{
public:
	typedef std::shared_ptr<LogAppender> ptr;
	virtual ~LogAppender(){} 
	virtual void log(std::shared_ptr<Logger> ptr,LogLevel::Level level, LogEvent::ptr event) = 0;

	LogAppender():m_level(sylar::LogLevel::Level::DEBUG){}
	void setFormatter(LogFormatter::ptr format){m_formatter = format;}
	LogFormatter::ptr getFormatter()const {return m_formatter;}

	void setLevel(LogLevel::Level level) { m_level = level;}
	LogLevel::Level  getLevel() const {return m_level;}
protected:
	LogFormatter::ptr m_formatter;
	LogLevel::Level m_level;
};


class Logger :public std::enable_shared_from_this<Logger>{
friend class  LoggerManager;
public:
	typedef std::shared_ptr<Logger> ptr;
	Logger(const std::string& name = "root");
	void log(LogLevel::Level level, LogEvent::ptr event);
	
	void debug(LogEvent::ptr event);
	void info(LogEvent::ptr event);
	void warn(LogEvent::ptr event);
	void error(LogEvent::ptr event);
	void fatal(LogEvent::ptr event);

	void addAppender(LogAppender::ptr appender);
	void delAppender(LogAppender::ptr appender);

	LogLevel::Level getLevel(){return m_level;}
	void setLevel(LogLevel::Level level) {m_level = level;}
	std::string getName()const {return m_name;}
private:
	std::string m_name;
	LogLevel::Level m_level;
	std::list<LogAppender::ptr> m_appenders;
	LogFormatter::ptr m_formatter;
	Logger::ptr m_root;
};

class StdoutLogAppender : public LogAppender
{
public:
	typedef std::shared_ptr<StdoutLogAppender> ptr;
	void log(std::shared_ptr<Logger> ptr,LogLevel::Level,LogEvent::ptr event) override;
private:
};

class FileLogAppender : public LogAppender
{
public:
	typedef std::shared_ptr<FileLogAppender> ptr;
	FileLogAppender(const std::string& filename);
	void log(std::shared_ptr<Logger> ptr,LogLevel::Level level, LogEvent::ptr event) override;
	bool reopen();
private:
	std::string m_filename;
	std::ofstream m_filestream;
	
};

class LoggerManager{
public:
	LoggerManager();
	Logger::ptr getLogger(const std::string& logger);
	void init();
	Logger::ptr getRoot()const {return m_root;}
private:
	std::map<std::string,Logger::ptr> m_loggers;
	Logger::ptr m_root;

};

typedef  sylar::Singleton<LoggerManager> LoggerMgr;

}


#endif
