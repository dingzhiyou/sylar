#include "config.h"
#include <sstream>
#include <utility>
#include<list>
#include<string>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>

namespace sylar{
ConfigVarBase::ptr Config::LookupBase(const std::string& name){
        auto it = GetDatas().find(name);
        if(it != GetDatas().end()){
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

void Config::LoadFromConDir(const std::string& path){
	std::string absolute_path = sylar::EnvMgr::GetInstance()->getAbsolutePath(path);
	std::vector<std::string> files;
	FSUtil::ListAllFiles(files,absolute_path,".yml");
	for(auto i : files){
		try{
			YAML::Node root = YAML::LoadFile(i);
			LoadFromYaml(root);
			SYLAR_LOG_INFO(g_logger) << "LoadFromYaml file: "<<i;
		}catch(...){
			SYLAR_LOG_ERROR(g_logger) << "LoadConfig File "<< i
						  << " faild";
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
