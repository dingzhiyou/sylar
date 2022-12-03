#include<iostream>
#include"../sylar/util.h"
#include "../sylar/log.h"

int main(int argc,char** argv)
{
	sylar::Logger::ptr  logger(new sylar::Logger);
	sylar::LogAppender::ptr  fileappend(new sylar::FileLogAppender("./log"));
	fileappend->setLevel(sylar::LogLevel::Level::ERROR);

	logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));
	logger->addAppender(sylar::LogAppender::ptr(fileappend));
	SYLAR_LOG_DEBUG(logger)<<"hello dzy";
	SYLAR_LOG_DEBUG(logger)<<"hello dzy";
	SYLAR_LOG_INFO(logger)<<"hello dzy";
	SYLAR_LOG_WARN(logger)<<"hello dzy";
	SYLAR_LOG_ERROR(logger)<<"hello dzy";
	SYLAR_LOG_ERROR(logger)<<"hello world";


	auto it = sylar::LoggerMgr::GetInstance()->getLogger("xxxx");
	SYLAR_LOG_ERROR(it) << "kkkkkk";
}
