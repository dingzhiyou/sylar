#include "../sylar/config.h"
#include "../sylar/log.h"
#include <sstream>
#include "../sylar/env.h"
#include <vector>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include<yaml-cpp/yaml.h>

//sylar::ConfigVar<std::vector<int> >::ptr g_int_v_value = sylar::Config::Lookup("system.int_set",std::vector<int>{1,2},"sylar int_vec");
/*
//sylar::ConfigVar<float>::ptr g_float_value = sylar::Config::Lookup("sylar.value",(float)8080,"sylar value");
sylar::ConfigVar<std::list<int> >::ptr g_int_list_value = sylar::Config::Lookup("system.int_list",std::list<int>{3,4},"sylar list_vec");
sylar::ConfigVar<std::set<int> >::ptr g_int_set_value = sylar::Config::Lookup("system.int_set",std::set<int>{1,2},"sylar int_vec");
sylar::ConfigVar<std::map<std::string,int> >::ptr g_int_map_value = sylar::Config::Lookup("system.int_map",std::map<std::string,int>{{"222",3}},"sylar int_map");
//sylar::ConfigVar<std::unordered_set<int> >::ptr g_unordered_set_value = sylar::Config::Lookup("system.int_unordered_set"
//		,std::unordered_set<int>{1,2},"sylar int_vec");
sylar::ConfigVar<std::unordered_map<std::string , int> >::ptr g_unordered_map_value = sylar::Config::Lookup("system.int_unordered_map",std::unordered_map<std::string,int>{{"k",2}},"sylar int_vec");
void print_yaml(const YAML::Node& node,int level){
	if(node.IsScalar()){
		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<std::string(level * 4 ,' ') 
						<<node.Scalar()<<" - "<<node.Tag() << " - "<<level;
	}else if(node.IsNull()){
		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4,' ')
						<<" nullptr "<<" - "<<node.Tag()<<" - "<<level;
	}else if(node.IsMap()){
		for(auto it = node.begin();
			it != node.end();
			it++){
			SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ') 
							 << it->first <<" - "<<it->second.Tag() << " - "<<level;
			print_yaml(it->second,level + 1);
		}

	}else if(node.IsSequence()){
		for(size_t i = 0;i < node.size();i++){
			SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ') 
							 << i <<" - " <<node[i].Tag()<< " - "<<level;
			print_yaml(node[i], level + 1);
		}
	
	}



}
*/


/*
void test_yaml(){
	YAML::Node root = YAML::LoadFile("/home/dada/workspace/sylar/bin/conf/test.yml");
	SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << root;
	sylar::Config::LoadFromYaml(root);
}

*/



/*
class Person{
public:
	int age = 0;
	std::string name = "mmm" ;
};


sylar::ConfigVar<Person>::ptr g_person = sylar::Config::Lookup("system.person",Person(),"sylar int_vec");


namespace sylar{

template<>
class LexicalCast<std::string, Person>{
public:
	Person operator()(const std::string& v){
		Person p;
		YAML::Node node = YAML::Load(v);
		p.name = node["name"].as<std::string>();
		p.age = node["age"].as<int>();
	        return p;
	}

};

template<>
class LexicalCast<Person,std::string>{
public:

	std::string operator()(const Person& p){
		YAML::Node node;
		node["name"] = p.name;
		node["age"] = p.age;

		std::stringstream ss;
		ss<<node;
	       return ss.str();	
	}

};

}
*/
//sylar::ConfigVar<int>::ptr g_int_value = sylar::Config::Lookup("sylar.port",(int)8080,"sylar port");

void test_confDir(){
	sylar::Config::LoadFromConDir("conf");
}
int main(int argc,char** argv)
{
	sylar::EnvMgr::GetInstance()->init(argc,argv);
	test_confDir();
	//SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_int_value->getValue();
	//SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "use count " << g_int_value.use_count();
	//SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_float_value->toString();

//	auto x = g_int_list_value->getValue();
//	for(auto i : x){
//		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "list_int: "<<i;
//	}
//	auto x2 = g_int_set_value->getValue();
//	for(auto i : x2){
//		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "set_int: "<<i;
//	}
//	auto x5 = g_unordered_map_value->getValue();
//	for(auto i : x5){
//		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << i.first <<" - "<<i.second;
//	}
	//test_yaml();
	//auto it = g_int_vec_value->getValue();
	//for(auto i : it){
	//	SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< "int_vec: " <<i;
	//}
//	auto x1 = g_int_list_value->getValue();
//	for(auto i : x1){
//		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "list_int: "<<i;
//	}
//	auto x4 = g_int_set_value->getValue();
//	for(auto i : x4){
//		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "set_int: "<<i;
//	}
//	auto x6 = g_unordered_map_value->getValue();
//	for(auto i : x6){
//		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << i.first <<" - "<<i.second;
//	}	
//	 m1 = g_int_map_value->getValue();
//	for(auto i : m1){		
//		SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< "int_map: "<<i.first <<"---"<<i.second;
//	}
//
//	 p = g_person->getValue();
//	SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< "g_person: "<<p.name;
//	SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< "g_person: "<<p.age;
	return 0;

}
