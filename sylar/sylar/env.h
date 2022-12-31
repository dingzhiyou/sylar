#ifndef __SYLAR_ENV_H__
#define __SYLAR_ENV_H__

#include"singleton.h"
#include"thread.h"
#include<map>
#include<vector>


namespace sylar{

class Env{
public:
	typedef RWMutex MutexType;

	bool init(int argc,char** argv);
	void add(const std::string& key,const std::string& val);
	std::string get(const std::string& key,const std::string& val = "");
	bool has(const std::string& key);
	void del(const std::string& key);

	void addHelp(const std::string& key,const std::string& val);
	bool removeHelp(const std::string& key);
	void printHlep();
	void setEnv(const std::string& key,const std::string& val);
	const std::string getEnv(const std::string& key,const std::string& default_val = "");
	std::string getCWD()const {return m_cwd;}
	std::string getAbsolutePath(const std::string& path);
private:
	MutexType m_mutex;
	std::map<std::string, std::string> m_args;
	std::vector<std::pair<std::string,std::string> > m_helps;
	std::string m_program;
	std::string m_exe;
	std::string m_cwd;
};

typedef Singleton<Env> EnvMgr;

}



#endif
