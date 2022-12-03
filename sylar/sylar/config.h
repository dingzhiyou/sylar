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
namespace sylar{

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
			SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "ss.str() : "<<ss.str(); 
			vec.push_back(LexicalCast<std::string,T>()(ss.str()));
		}

		for(auto i : vec){
			SYLAR_LOG_DEBUG(SYLAR_LOG_ROOT()) <<"vec: " <<i;
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
			SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "ss.str() : "<<ss.str(); 
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
			SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "ss.str() : "<<ss.str(); 
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
			std::cout<<"it->first"<<it->first << "       -       "<<it->second;


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
		auto it = m_datas.find(name);
		if( it != m_datas.end()){
			auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
			if(tmp){
				SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "tmp.use_count"<< tmp.use_count();
				return tmp;
			}else{
				SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "m_datas name: " << name << "  already exist";
			}
		}

		typename ConfigVar<T>::ptr v(new ConfigVar<T>(name,default_val,description));
		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "v.use_count"<< v.use_count();
		m_datas[name] = v;
		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "v.use_count"<< v.use_count();
		return v;
	}

	template<class T>
	static typename ConfigVar<T>::ptr Lookup(const std::string& name){
		auto it = m_datas.find(name);
		if(it == m_datas.end()){
			return nullptr;
		}
		return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);

	}


	static void LoadFromYaml(YAML::Node root);
	static ConfigVarBase::ptr LookupBase(const std::string& name);
private:
	static ConfigMap m_datas;

};
Config::ConfigMap Config::m_datas;


ConfigVarBase::ptr Config::LookupBase(const std::string& name){
        auto it = m_datas.find(name);
        if(it != m_datas.end()){
                return it->second;
        }
        return nullptr;

}



static void ListAllMember(std::string prefix,const YAML::Node& node, std::list<std::pair<std::string,const YAML::Node> >& output){
        output.push_back(std::make_pair(prefix,node));
        if(node.IsMap()){
                for(auto it = node.begin();it != node.end();it++){
                        ListAllMember(prefix.empty() ? it->first.Scalar() : (prefix + "." + it->first.Scalar()) , it->second , output);
                }
        }

}


void Config::LoadFromYaml(YAML::Node root){
        std::list<std::pair<std::string,const YAML::Node> > all_nodes;
        std::string str("");
        ListAllMember(str,root,all_nodes);

        for(auto it : all_nodes){
                std::string key  = it.first;
                if(key.empty()){
                        continue;
                }
                ConfigVarBase::ptr val = LookupBase(it.first);
                if(val){


                        if(it.second.IsScalar()){
                                std::string str(it.second.Scalar());
                        }else{
                                std::stringstream ss;
                                ss << it.second;
                                std::string str(ss.str());
                                val->fromString(str);
                        }
                }


        }
}

}



#endif
