#include "config.h"
#include "log.h"
#include <cctype>
#include <ctime>
#include<string.h>
#include <functional>
#include<iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <map>
namespace sylar{
LoggerManager::LoggerManager()
{
	m_root.reset(new Logger);
	m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
}
void LoggerManager::init(){


}

struct LogAppenderDefine{
	int type = 1;
	LogLevel::Level level = LogLevel::Level::UNKNOW;
	std::string formatter;
	std::string file;

	bool operator==(const LogAppenderDefine& oth) const{
		return type == oth.type && level == oth.level
			&& formatter == oth.formatter
			&& file == oth.file;
	}
};

struct LogDefine{
	std::string name;
	LogLevel::Level level = LogLevel::Level::UNKNOW;
	std::string formatter;
	std::vector<LogAppenderDefine> appenders;


	bool operator==(const LogDefine& oth){
		return name == oth.name
			&& level == oth.level
			&& formatter == oth.formatter
			&& appenders == oth.appenders;
	}
	bool operator<(const LogDefine& oth){
		return name <oth.name;
	}
};

//sylar::ConfigVar<std::set<LogDefine> >::ptr  g_log_define = sylar::Config::Lookup("logs",std::set<LogDefine>(),"log config");


Logger::ptr LoggerManager::getLogger(const std::string& name)
{
	auto it  = m_loggers.find(name);
	if(it != m_loggers.end()){
		return it->second;
	}
	Logger::ptr log(new Logger(name));
	log->m_root = m_root;
	m_loggers[name] = log;
	return log;


}
	
class NewLineFormatItem : public LogFormatter::FormatItem{
public:
	NewLineFormatItem(const std::string& str=""){}
	void  format(std::shared_ptr<Logger> ptr,std::stringstream& ss
			,LogLevel::Level level ,LogEvent::ptr event)override
	{
		ss << std::endl;
	}
};


class StringFormatItem: public LogFormatter::FormatItem{
public:
	StringFormatItem(const std::string& str)
			:m_string(str){}
	void format(std::shared_ptr<Logger> ptr,std::stringstream& ss,LogLevel::Level level,LogEvent::ptr event)override
	{
		ss << m_string;
	}

private:
	std::string m_string;
};
class MessageFormatItem : public LogFormatter::FormatItem{
public:
	MessageFormatItem(const std::string& str=""){}
	void  format(std::shared_ptr<Logger> ptr,std::stringstream& ss
			,LogLevel::Level level ,LogEvent::ptr event)override
	{
		ss << event->getContent();
	}
};
class LevelFormatItem: public LogFormatter::FormatItem{
public:
	LevelFormatItem(const std::string& str=""){}

	void format(std::shared_ptr<Logger> ptr,std::stringstream& ss,
		    LogLevel::Level level,LogEvent::ptr event)override
	{
		ss << LogLevel::toString(level);
	}
};
class ElapseFormatItem: public LogFormatter::FormatItem{
public:
		
	ElapseFormatItem(const std::string& str=""){}
	void format(std::shared_ptr<Logger> ptr,std::stringstream& ss
			,LogLevel::Level level,LogEvent::ptr event)override
	{
		ss << event->getElapse();
	}
};
class NameFormatItem: public LogFormatter::FormatItem{
public:
	NameFormatItem(const std::string& str=""){}
	void format(std::shared_ptr<Logger> ptr,std::stringstream& ss
			,LogLevel::Level level,LogEvent::ptr event)override
	{
		ss << event->getLogger()->getName();
	}
};


class ThreadIdFormatItem: public LogFormatter::FormatItem{
public:
	ThreadIdFormatItem(const std::string& str=""){}
	void format(std::shared_ptr<Logger> ptr,std::stringstream& ss
			,LogLevel::Level level,LogEvent::ptr event)override
	{
		ss << event->getThreadId();
	}
};
class FiberIdFormatItem: public LogFormatter::FormatItem{
public:
	FiberIdFormatItem(const std::string& str=""){}
	void format(std::shared_ptr<Logger> ptr,std::stringstream& ss
			,LogLevel::Level level,LogEvent::ptr event)override
	{
		ss << event->getFriberId();
	}
};


class DateTimeFormatItem: public LogFormatter::FormatItem{
public:

	DateTimeFormatItem(const std::string format="%Y-%m-%d %H:%M:%s")
		:m_format(format){
			
		if(m_format.empty())
		{
			m_format = std::string("%Y-%m-%d %H:%M:%s",strlen("%Y-%m-%d %H:%M:%s"));
			
		}
	}

	void format(std::shared_ptr<Logger> ptr,std::stringstream& ss
			,LogLevel::Level level,LogEvent::ptr event)override
	{
		struct tm tm;
		time_t time = event->getTime();
		localtime_r(&time,&tm);
		char buf[64];
		strftime(buf,sizeof(buf),m_format.c_str() ,&tm);
		ss << buf;
	}

private:
	std::string m_format;
};

class FilenameFormatItem: public LogFormatter::FormatItem{
public:
	FilenameFormatItem(const std::string& str=""){}
	void format(std::shared_ptr<Logger> ptr,std::stringstream& ss
			,LogLevel::Level level,LogEvent::ptr event)override
	{
		ss << event->getFile();
	}
};

class LineFormatItem: public LogFormatter::FormatItem{
public:
	LineFormatItem(const std::string& str=""){}
	void format(std::shared_ptr<Logger> ptr,std::stringstream& ss
			,LogLevel::Level level,LogEvent::ptr event)override
	{
		ss << event->getLine();
	}
};
class TabFormatItem: public LogFormatter::FormatItem{
public:
	TabFormatItem(const std::string& str=""){}
	void format(std::shared_ptr<Logger> ptr,std::stringstream& ss
			,LogLevel::Level level,LogEvent::ptr event)override
	{
		ss <<"\t";
	}
};

const char* LogLevel::toString(LogLevel::Level level)
{
	switch (level) {
#define XX(name) \
	case LogLevel::Level::name:  \
	return #name; \
	break; 
	
	XX(DEBUG);
	XX(INFO);
	XX(WARN);
	XX(ERROR);
	XX(FATAL);

	default:
	return "KNOW";
#undef XX
	return "KNOW";
/*

	      case LogLevel::Level::DEBUG:
	      return "DEBUG";
	      break;
	      case LogLevel::Level::INFO:
	      return "INFO";
	      break;
	      case LogLevel::Level::ERROR:
	      return "ERROR";
	      break;
	      case LogLevel::Level::FATAL:
	      return "FATAL";
	      break;
	      case LogLevel::Level::WARN:
	      return "WARN";
	      break;
	  default:
	      return "KNOW";
*/
	}
}


LogEventWarp::~LogEventWarp(){
		m_event->getLogger()->log(m_event->getLevel(),m_event);
}



LogFormatter::LogFormatter(const std::string& pattern)
	:m_pattern(pattern)
{
	init();
}
std::string  LogFormatter::format(std::shared_ptr<Logger> ptr,LogLevel::Level level,LogEvent::ptr event)
{
	std::stringstream ss;
	for(auto& i : m_items)
	{
		i->format(ptr,ss,level,event);
	}
	return ss.str();
}

void LogFormatter::init()
{
	std::vector<std::tuple<std::string,std::string,int> > vec;
  	std:: string nstr;
	for(size_t i = 0; i< m_pattern.size();++i)
	{
		if(m_pattern[i] != '%')
		{
			nstr.append(1,m_pattern[i]);
			continue;
		}

		if((i + 1) < m_pattern.size())
		{
			if(m_pattern[i+1] == '%')
			{
				nstr.append(1,'%');
				continue;
			}
		}

		size_t n  = i+1;
		int fmt_status = 0;
		size_t fmt_begin = 0;

		std::string str;
		std::string fmt;
		while(n < m_pattern.size())
		{
			if(!std::isalpha(m_pattern[n]) && m_pattern[n] != '{'
					&& m_pattern[n] != '}')
			{
				break;
			}

			if(fmt_status == 0)
			{
				if(m_pattern[n] == '{')
				{
					str = m_pattern.substr(i+1,n-i-1);
					fmt_status = 1;
					fmt_begin = n;
					++n;
					continue;
				}
			}
			if(fmt_status == 1)
			{
				if(m_pattern[n] == '}')
				{
					fmt = m_pattern.substr(fmt_begin+1,n-fmt_begin - 1);
					fmt_status = 2;
					break;
				}
			}
			++n;
		}

		if(fmt_status == 0)
		{
			if(!nstr.empty())
			{
				vec.push_back(std::make_tuple(nstr,std::string(),0));
				nstr.clear();
			}
			str = m_pattern.substr(i+1,n-i-1);
			vec.push_back(std::make_tuple(str,fmt,1));
			i = n - 1;
		}else if(fmt_status == 1)
		{
			std::cout<<"pattern prase error : "<<m_pattern << " - "<<m_pattern.substr(i)<<std::endl;
    			vec.push_back(std::make_tuple("<<pattern_error>>",fmt,0));
		}
		else if(fmt_status == 2)
		{
			if(!nstr.empty())
			{
				vec.push_back(std::make_tuple(nstr,"",0));
				nstr.clear();
			}
			vec.push_back(std::make_tuple(str,fmt,1));
			i = n - 1;
		}
	}
	if(!nstr.empty())
	{
		vec.push_back(std::make_tuple(nstr,"",0));
	}

		static std::map<std::string,std::function<FormatItem::ptr(const std::string& str)> >
			s_format_items = {
			#define XX(str,C) \
			{#str,[](const std::string& fmt){return FormatItem::ptr(new C(fmt));}}	
				XX(m,MessageFormatItem),
				XX(p,LevelFormatItem),
				XX(r,ElapseFormatItem),
				XX(c,NameFormatItem),
				XX(T,TabFormatItem),
				XX(t,ThreadIdFormatItem),
				XX(n,NewLineFormatItem),
				XX(d,DateTimeFormatItem),
				XX(f,FilenameFormatItem),
				XX(F,FiberIdFormatItem),
				XX(l,LineFormatItem),
			#undef  XX
		};


		for(auto& i : vec)
		{
			if(std::get<2>(i) == 0)
			{
				m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
			}
			else
			{
				auto it = s_format_items.find(std::get<0>(i));
				if(it == s_format_items.end())
				{
					m_items.push_back(FormatItem::ptr(new StringFormatItem("<<format error>>" 
									+ std::get<0>(i) + ">>")));
				}
				else
				{
					m_items.push_back(it->second(std::get<1>(i)));
				}
				
			}
		}
}




void StdoutLogAppender::log(std::shared_ptr<Logger> ptr,LogLevel::Level level, LogEvent::ptr event)
{

	if(m_level <= level)
	{
		std::cout<<m_formatter->format(ptr,level,event);
	}	
}

FileLogAppender::FileLogAppender(const std::string& filename)
	:m_filename(filename)
{
	reopen();
}
void FileLogAppender::log(std::shared_ptr<Logger> ptr,LogLevel::Level level, LogEvent::ptr event)
{
	if(level >= m_level)
	{
		m_filestream << m_formatter->format(ptr,level,event);
	}
}
bool FileLogAppender::reopen()
{
	if(m_filestream)
	{
		m_filestream.close();
	}
	m_filestream.open(m_filename);
	return !!m_filestream ;
}
LogEvent::LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char* file
		  , uint32_t line, uint32_t elapse , uint32_t threadid, uint32_t friberid
                 ,uint64_t time):
	m_file(file)
	,m_line(line)
	,m_elapse(elapse)
	,m_threadId(threadid)
	,m_fiberId(friberid)
	,m_time(time)
	,m_logger(logger)
	,m_level(level)
{
	
}

Logger::Logger(const std::string& name):
	m_level(LogLevel::Level::DEBUG)
{
	m_name = name;
	m_formatter.reset(new LogFormatter("%d%T[%c]    [%p]%T%T%F%T%t%T<%f:%l>%T%m%T%n"));
}
void Logger::log(LogLevel::Level level, LogEvent::ptr event)
{
	auto it =shared_from_this(); 
	if(level >= m_level)
	{
		if(!m_appenders.empty()){
			for(auto& i : m_appenders)
			{
				i->log(it,level,event);
			}
		}else if(m_root){
			m_root->log(level,event);
		}

	}

}

void Logger::debug(LogEvent::ptr event)
{
	log(LogLevel::DEBUG,event);
}
void Logger::info(LogEvent::ptr event)
{
	log(LogLevel::INFO,event);
}
void Logger::warn(LogEvent::ptr event)
{
	log(LogLevel::WARN,event);
}
void Logger::error(LogEvent::ptr event)
{
	log(LogLevel::ERROR,event);
}
void Logger::fatal(LogEvent::ptr event)
{
	log(LogLevel::FATAL,event);
}

void Logger::addAppender(LogAppender::ptr appender)
{
	if(!appender->getFormatter())
	{
		appender->setFormatter(m_formatter);
	}
	
	m_appenders.push_back(appender);
}
void Logger::delAppender(LogAppender::ptr appender)
{
	for(auto it = m_appenders.begin();
			it != m_appenders.end();it++)
	{
		if((*it) == appender)
		{
			m_appenders.erase(it);
			break;
		}
	}
}




}
