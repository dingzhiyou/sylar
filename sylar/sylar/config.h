#ifndef __SYLAR_CONFIG_H__
#define __SYLAR__CONFIG_H__

#include<iostream>
#include <boost/exception/exception.hpp>
#include <boost/mpl/assert.hpp>
#include <vector>
#include <list>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include<yaml-cpp/yaml.h>
#include <boost/type.hpp>
#include<map>
#include<unordered_map>
#include<unordered_set>
#include <exception>
#include<memory>
#include<sstream>
#include<boost/lexical_cast.hpp>
#include <sys/types.h>
#include"log.h"
#include "env.h"
#include "util.h"
namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
static sylar::Logger::ptr GetGlog(){
	return g_logger;
}

class ConfigVarBase{
public:
	typedef std::shared_ptr<ConfigVarBase> ptr;
	ConfigVarBase(const std::string& name,const std::string& desc = "")
		:m_name(name)
		,m_description(desc)
	{ }

	const std::string& getName()const  {return m_name;}
	const std::string& getDescription()const {return m_description;}

	virtual ~ConfigVarBase(){}
	virtual std::string toString() = 0;
	virtual bool fromString(std::string& val) = 0;
private:
	std::string m_name;
	std::string m_description;
};

template<class F,class T>
class LexicalCast{
public:
	T operator()(const F& v){
			return boost::lexical_cast<T>(v);
	}
private:
};

template<class T>
class LexicalCast<std::string,std::vector<T> >{
public:
	std::vector<T> operator()(const std::string& v){
		YAML::Node node = YAML::Load(v);
		typename std::vector<T> vec;
		std::stringstream ss;
		for(size_t i = 0;i < node.size(); i++){
			ss.str("");
			ss << node[i];
			vec.push_back(LexicalCast<std::string,T>()(ss.str()));
		}
		return vec;
	}

};



template<class T>
class LexicalCast<std::vector<T>,std::string >{
public:
	std::string operator()(const std::vector<T>& v){
		YAML::Node node;
		for(size_t i = 0;i < v.size(); i++){
			node.push_back(YAML::Load(LexicalCast<T,std::string>()(v[i])));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}

};


template<class T>
class LexicalCast<std::string,std::list<T> >{
public:
	std::list<T> operator()(const std::string& v){
		YAML::Node node = YAML::Load(v);
		typename std::list<T> vec;
		std::stringstream ss;
		for(size_t i = 0;i < node.size(); i++){
			ss.str("");
			ss << node[i];
			vec.push_back(LexicalCast<std::string,T>()(ss.str()));
		}
		return vec;
	}
};



template<class T>
class LexicalCast<std::list<T> , std::string >{
public:
	std::string operator()(const std::list<T>& v){
		YAML::Node node;
		for(auto i : v){
			node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}

};



template<class T>
class LexicalCast<std::string,std::set<T> >{
public:
	std::set<T> operator()(const std::string& v){
		YAML::Node node = YAML::Load(v);
		typename std::set<T> vec;
		std::stringstream ss;
		for(size_t i = 0;i < node.size(); i++){
			ss.str("");
			ss << node[i];
			vec.insert(LexicalCast<std::string,T>()(ss.str()));
		}
		return vec;
	}

};



template<class T>
class LexicalCast<std::set<T> , std::string >{
public:
	std::string operator()(const std::set<T>& v){
		YAML::Node node;
		for(auto i : v){
			node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}

};


template<class T>
class LexicalCast<std::string,std::unordered_set<T> >{
public:
	std::unordered_set<T> operator()(const std::string& v){
		YAML::Node node = YAML::Load(v);
		typename std::unordered_set<T> vec;
		std::stringstream ss;
		for(size_t i = 0;i < node.size(); i++){
			ss.str("");
			ss << node[i];
			SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "ss.str() : "<<ss.str(); 
			vec.insert(LexicalCast<std::string,T>()(ss.str()));
		}
		return vec;
	}

};



template<class T>
class LexicalCast<std::unordered_set<T> , std::string >{
public:
	std::string operator()(const std::unordered_set<T>& v){
		YAML::Node node;
		for(auto i : v){
			node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}

};

template<class T>
class LexicalCast<std::string,std::unordered_map<std::string,T> >{
public:
	std::unordered_map<std::string, T> operator()(const std::string& v){
		YAML::Node node = YAML::Load(v);
		typename std::unordered_map<std::string,T> vec;
		std::stringstream ss;
		for(auto it = node.begin();it != node.end(); it++){
			ss.str("");
			ss << it->second;
			vec.insert(std::make_pair(it->first.Scalar(),LexicalCast<std::string,T>()(ss.str())));
		}
		return vec;
	}

};


template<class T>
class LexicalCast<std::unordered_map<std::string,T> , std::string >{
public:
	std::string operator()(const std::unordered_map<std::string,T>& v){
		YAML::Node node;
		for(auto i : v){
			node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}

};


template<class T>
class LexicalCast<std::string,std::map<std::string,T> >{
public:
	std::map<std::string, T> operator()(const std::string& v){
		YAML::Node node = YAML::Load(v);
		typename std::map<std::string,T> vec;
		std::stringstream ss;
		for(auto it = node.begin();it != node.end(); it++){
			ss.str("");
			ss << it->second;
			std::cout<<"it->first"<<it->first << "       -       "<<it->second;


			vec.insert(std::make_pair(it->first.Scalar(),LexicalCast<std::string,T>()(ss.str())));
		}
		return vec;
	}

};


template<class T>
class LexicalCast<std::map<std::string,T> , std::string >{
public:
	std::string operator()(const std::map<std::string,T>& v){
		YAML::Node node;
		for(auto i : v){
			node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
		}

		std::stringstream ss;
		ss << node;
		return ss.str();
	}

};




template<class T,class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string> >
class ConfigVar :public ConfigVarBase{
public:
	typedef std::shared_ptr<ConfigVar> ptr;
	ConfigVar(const std::string name,T default_val, const std::string& desc="")
		:ConfigVarBase(name,desc)
		,m_val(default_val)
		{}


	std::string toString() override{
		try{
		        //return boost::lexical_cast<std::string>(m_val);
			return ToStr()(m_val);
		}
		catch(std::exception& e)
		{
			SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception"
				<<e.what()<< " convert: "<<typeid(m_val).name() <<" to string";
		}
		return "";
	}

	bool fromString(std::string& val) override{
		try{
			//m_val = boost::lexical_cast<T>(val);
			setValue(FromStr()(val));
			return true;
		}catch(std::exception& e){
			SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::to " << typeid(m_val).name() << "exception"
				<<e.what()<< " convert: "<<typeid(m_val).name() <<" from string";
		}
		return false;
	}


	const T getValue() const {
		return m_val;
	}
	void setValue(const T& val){
		m_val = val;
	}
private:
	T m_val;

};

class Config{
public:
	typedef std::shared_ptr<Config> ptr;
	typedef std::map<std::string ,ConfigVarBase::ptr> ConfigMap;

	template<class T>
	static typename ConfigVar<T>::ptr Lookup(const std::string& name ,const T& default_val
			,const std::string& description = ""){
		auto it = GetDatas().find(name);
		if( it != GetDatas().end()){
			auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
			if(tmp){
				return tmp;
			}else{
				SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "m_datas name: " << name << "  already exist";
			}
		}

		typename ConfigVar<T>::ptr v(new ConfigVar<T>(name,default_val,description));
		GetDatas()[name] = v;
		return v;
	}

	template<class T>
	static typename ConfigVar<T>::ptr Lookup(const std::string& name){
		auto it = GetDatas().find(name);
		if(it == GetDatas().end()){
			return nullptr;
		}
		return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);

	}


	static void LoadFromYaml(YAML::Node root);
	static void LoadFromConDir(const std::string& path);
	static ConfigVarBase::ptr LookupBase(const std::string& name);
	static ConfigMap& GetDatas(){
		static ConfigMap m_datas;
		return m_datas;
	}
private:

};




}



#endif
