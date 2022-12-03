#include "ipaddr.h"
#include"log.h"
#include "endian.h"
#include <cstdlib>
#include <memory>
#include <netdb.h>
#include <cstddef>
#include<arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/un.h>
#include <vector>
#include<ifaddrs.h>
namespace sylar{

template <class T>
static uint32_t CountBytes(T value){
	uint32_t result  =0;
	for(;value;result++){
		value &= value - 1;
	}

	return result;
} 

bool Address::GetInterfaceAddress(std::multimap<std::string,std::pair<Address::ptr,uint32_t> >& result,int family){
	struct ifaddrs *next,*results;

	if(getifaddrs(&results) != 0){
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Address getifaddrs error";
		return false;
	}

	try{
		for(next = results;next;next = next->ifa_next){
			Address::ptr addr;
			uint32_t prefix_len = ~0u;
			if(family != AF_UNSPEC && family != next->ifa_addr->sa_family){
				continue;
			}
			switch(next->ifa_addr->sa_family){
				case AF_INET:
					{
						addr = Create(next->ifa_addr,sizeof(sockaddr_in));
						uint32_t netmask = ((sockaddr_in*)(next->ifa_netmask))->sin_addr.s_addr;
						prefix_len = CountBytes(netmask);
					}
					break;
			   	case AF_INET6:
					{
						addr = Create(next->ifa_addr,sizeof(sockaddr_in6));
						in6_addr& netmask = ((sockaddr_in6*)(next->ifa_netmask))->sin6_addr;
if(!addr){
							SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "OKOKOKOK";
						}
						prefix_len = 0;
						for(int i = 0; i < 16;i++){
							prefix_len += CountBytes(netmask.s6_addr[i]);
						}
				
					}
				break;
				default:
				break;
			
			}
			if(addr){
				result.insert(std::make_pair(next->ifa_name,std::make_pair(addr,prefix_len)));
			}
		}
	}catch(...){
		freeifaddrs(results);
		return false;
	}	

	freeifaddrs(results);
	return true;


}
	

bool Address::GetInterfaceAddress(std::vector<std::pair<Address::ptr,uint32_t> >& result,const std::string& iface,int family){
	if(iface.empty() || iface== "*"){
		if(family == AF_INET || family == AF_UNSPEC){
			result.push_back(std::make_pair(Address::ptr(new IPv4Address()),0u));
		}
		if(family == AF_INET6 || family == AF_UNSPEC){
			result.push_back(std::make_pair(Address::ptr(new IPv6Address()),0u));
		}
		return true;
	}

	std::multimap<std::string,std::pair<Address::ptr,uint32_t> > results;
	if(GetInterfaceAddress(results,family)){
		return false;		
	}
	auto its = results.equal_range(iface);
	for(;its.first != its.second;++its.first){
		result.push_back(its.first->second);
	}
	return true;

}


bool Address::Lookup(std::vector<Address::ptr>& vec,const std::string& host,int family,int type,int protocol){
	addrinfo hints, *results,*next;
	hints.ai_flags = 0;
	hints.ai_family = family;
	hints.ai_socktype = type;
	hints.ai_canonname = NULL;
	hints.ai_protocol = protocol;
	hints.ai_addr =NULL;
	hints.ai_next = NULL;

	std::string node;
	const char* service = NULL;
	if(!host.empty() && host[0] == '['){
		const char* endipv6 = (const char*)memchr(host.c_str() + 1,']',host.size() - 1);
		if(endipv6){
			//TODO check out of range
			if(*(endipv6 + 1) == ':'){
				service = endipv6 + 2;
			}
			node = host.substr(1,endipv6 - host.c_str() - 1);
		}
	}
	if(node.empty()){
		service = (const char*)memchr(host.c_str(),':',host.size());
		if(service){
			if(!memchr(service + 1,':',host.c_str()  + host.size() - service - 1)){
				node = host.substr(0,service - host.c_str());
				++service;
			}
		}
	}


	if(node.empty()){
		node = host;
	}
	int error = getaddrinfo(node.c_str(),service,&hints,&results);
	if(error){
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "getAddress::Lookup error host:  "<< host<<"type: " << type 
			                          <<"protocol: "<<protocol;
		return false;
	}
	next = results;
	while(next){
		vec.push_back(Address::Create(next->ai_addr,(socklen_t)next->ai_addrlen));
		next = next->ai_next;

	}

	freeaddrinfo(results);
	return true;
}

std::shared_ptr<IPAddress> Address::LookupAnyIPAddress(const std::string& host,int family,int type,int protocol){
	std::vector<Address::ptr> result;
	if(Address::Lookup(result,host,family,type, protocol)){
		for(auto& i : result){
			IPAddress::ptr v = std::dynamic_pointer_cast<IPAddress>(i);
			if(v){
				return v;
			}
		}
	}
	return nullptr;

}

Address::ptr Address::LookupAny(const std::string& host,int family,int type,int protocol){
	std::vector<Address::ptr> result;
	if(Address::Lookup(result,host,family,type, protocol)){
		return result[0];
	}
	return nullptr;


}
template<class T>
static T CreateMask(uint32_t bits){
	return (1 << (sizeof(T) * 8 - bits)) - 1;
}

UnknowAddress::UnknowAddress(const sockaddr addr){
	m_addr = addr;
}
Address::ptr Address::Create(const sockaddr* addr,socklen_t addrlen){
	if(!addr){
		return nullptr;
	}
	Address::ptr rt;
	switch (addr->sa_family) {
		case AF_INET:
			rt.reset(new IPv4Address(*(const sockaddr_in*)addr));
			break;
		case AF_INET6:
			rt.reset(new IPv6Address(*(const sockaddr_in6*)addr));
			break;

		default:
			rt.reset(new UnknowAddress(*addr));
	}
	return rt;
}
int Address::getFamily()const {
	return getAddr()->sa_family;
}
std::string Address::toString(){
	std::stringstream os;
	insert(os);
	return os.str();
}

bool Address::operator<(const Address& lhs) const { 
	socklen_t minlen = std::min(getAddrLen(),lhs.getAddrLen());
	int res = memcmp(getAddr(),lhs.getAddr(),minlen);
	if(res < 0){
		return true;
	}else{
		return false;
	}
}
bool Address::operator==(const Address& rhs) const{
	socklen_t minlen = std::min(getAddrLen(),rhs.getAddrLen());
	int res = memcmp(getAddr(),rhs.getAddr(),minlen);
	if(res == 0){
		return true;
	}else{
		return false;
	}
}


bool Address::operator!=(const Address& rhs)const{
	socklen_t minlen = std::min(getAddrLen(),rhs.getAddrLen());
	int res = memcmp(getAddr(),rhs.getAddr(),minlen);
	if(res == 0){
		return false;
	}else{
		return true;
	}
}

IPv4Address::ptr IPv4Address::Create(const char* address,uint16_t port){
	IPv4Address::ptr rt(new IPv4Address);
	rt->m_addr.sin_port = byteSwapOnLittleEndian(port);
	int res = inet_pton(AF_INET,address,&rt->m_addr.sin_addr);
	if(res < 0){
		SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "inet_pton err";
		return nullptr;
	}
	return rt;

}
IPv4Address::IPv4Address(uint32_t address,uint16_t port){
	std::memset(&m_addr,0,sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = byteSwapOnLittleEndian(port);
	m_addr.sin_addr.s_addr = byteSwapOnLittleEndian(address);
}


sockaddr* IPv4Address::getAddr()const{
	return	(sockaddr*)&m_addr;

}
socklen_t IPv4Address::getAddrLen() const {
	return (socklen_t)sizeof(m_addr);
}
std::ostream& IPv4Address::insert(std::ostream& os)const{
	uint32_t addr = byteSwapOnLittleEndian(m_addr.sin_addr.s_addr);
	os << ((addr >> 24) & 0xff) << "."
	   << ((addr >> 16) & 0xff) << "."
	   <<((addr >> 8) & 0xff) << "."
	   <<(addr & 0xff) <<":"<<byteSwapOnLittleEndian(m_addr.sin_port);

	return os;
}

IPv4Address::IPv4Address(const sockaddr_in& address){
	m_addr = address;
}
IPAddress::ptr IPv4Address::broadcastAddress(uint32_t prefix_len){
	if(prefix_len > 32){
		return nullptr;
	}

	sockaddr_in baddr(m_addr);
	baddr.sin_addr.s_addr |= byteSwapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
	IPv4Address::ptr rt(new IPv4Address(baddr));
	return rt;

}
IPAddress::ptr IPv4Address::networkAddress(uint32_t prefix_len){
	if(prefix_len > 32){
		return nullptr;
	}

	sockaddr_in baddr(m_addr);
	baddr.sin_addr.s_addr &= byteSwapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
	IPv4Address::ptr rt(new IPv4Address(baddr));
	return rt;



}
IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len){
	sockaddr_in subnet;
	memset(&subnet,0,sizeof(sockaddr_in));
	subnet.sin_family = AF_INET;
	subnet.sin_addr.s_addr = ~byteSwapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
	IPv4Address::ptr rt(new IPv4Address(subnet));
	return rt;
}	


uint32_t IPv4Address::getPort()const{
	return byteSwapOnLittleEndian(m_addr.sin_port);

}
void IPv4Address::setPort(uint16_t v){
	m_addr.sin_port = byteSwapOnLittleEndian(v);
}

IPv6Address::IPv6Address(){
	std::memset(&m_addr,0,sizeof(m_addr));
	m_addr.sin6_family = AF_INET6;
}

IPv6Address::IPv6Address(const uint8_t address[16],uint16_t port){
	std::memset(&m_addr,0,sizeof(m_addr));
	m_addr.sin6_family = AF_INET6;
	m_addr.sin6_port = byteSwapOnLittleEndian(port);
	memcpy(&m_addr.sin6_addr.s6_addr,address,16);
}
IPv6Address::ptr Create(const char* addr,uint16_t port){
	IPv6Address::ptr rt(new IPv6Address);
	std::memset((void*)rt->getAddr(),0,sizeof(*(rt->getAddr())));
	((sockaddr_in6*)rt->getAddr())->sin6_family = AF_INET6;
	((sockaddr_in6*)rt->getAddr())->sin6_port = byteSwapOnLittleEndian(port);
	int res = inet_pton(AF_INET6, addr,&((sockaddr_in6*)rt->getAddr())->sin6_addr);
	if(res <= 0){
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "inet_pton err";
	}
	return rt;
}

IPAddress::ptr IPAddress::Create(const char* address, uint16_t port){
	addrinfo hints,*results;
	memset(&hints,0,sizeof(addrinfo));

	int error = getaddrinfo(address,NULL,&hints,&results);
	if(error){
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT())  <<  "getaddrinfo error ";
	       return nullptr;	
	}
	try{
		IPAddress::ptr result(std::dynamic_pointer_cast<IPAddress>(Address::Create(results->ai_addr,(socklen_t)results->ai_addrlen)));
	if(result){
		result->setPort(port);
		freeaddrinfo(results);
		return result;
	}
	}catch(...){
		freeaddrinfo(results);
		return nullptr;
	}
	return nullptr;
}
sockaddr* IPv6Address::getAddr()const{
	return (sockaddr*)&m_addr;
}
socklen_t IPv6Address::getAddrLen() const{
	return (socklen_t)sizeof(m_addr);
}
std::ostream& IPv6Address::insert(std::ostream& os)const{
	os <<"[";
	uint16_t* addr = (uint16_t*)m_addr.sin6_addr.s6_addr;
	bool user_zero = false;
	for(size_t i = 0;i < 8;i++){
		if(addr[i] == 0 && !user_zero){
			continue;
		}
		if(i && addr[i-1] == 0 && !user_zero){
			os<< ":";
			user_zero = true;
		}
		if(i){
			os<<":";
		}
		os <<std::hex<< (int)byteSwapOnLittleEndian(addr[i]) << std::dec;
	}
	if(!user_zero && addr[7] == 0){
		os <<"::"; 
	}

	os<< "]"<<":" <<byteSwapOnLittleEndian(m_addr.sin6_port);
	return os;
}

IPv6Address::IPv6Address(const sockaddr_in6& address){
	m_addr = address;
}
IPAddress::ptr IPv6Address::broadcastAddress(uint32_t prefix_len){
	sockaddr_in6 baddr(m_addr);
	baddr.sin6_addr.s6_addr[prefix_len / 8] |= CreateMask<uint8_t>(prefix_len % 8);
	for(int i = prefix_len / 8 + 1 ; i < 16 ; i++){
		baddr.sin6_addr.s6_addr[i] = 0xff;
	}
 	IPv6Address::ptr rt(new IPv6Address(baddr));
	return rt;

}
IPAddress::ptr IPv6Address::networkAddress(uint32_t prefix_len){
	sockaddr_in6 baddr(m_addr);
	baddr.sin6_addr.s6_addr[prefix_len / 8] &= CreateMask<uint8_t>(prefix_len % 8);
	//for(int i = prefix_len / 8 + 1 ; i < 16 ; i++){
	//	baddr.sin6_addr.s6_addr[i] = 0xff;
	//}
 	IPv6Address::ptr rt(new IPv6Address(baddr));
	return rt;


}
IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len){
	sockaddr_in6 subaddr;
	memset(&subaddr,0,sizeof(subaddr));
	subaddr.sin6_family = AF_INET6;
	subaddr.sin6_addr.s6_addr[prefix_len / 8] = ~CreateMask<uint8_t>(prefix_len % 8);
	for(int i = prefix_len / 8 + 1 ; i < 16 ; i++){
		subaddr.sin6_addr.s6_addr[i] = 0xff;
	}
	return IPv6Address::ptr(new IPv6Address(subaddr));
 

}

uint32_t IPv6Address::getPort()const{
	return byteSwapOnLittleEndian(m_addr.sin6_port);
}
void IPv6Address::setPort(uint16_t v){
	m_addr.sin6_port = v;
}
static const size_t  MAX_PATH_LEN = sizeof(((sockaddr_un*)0)->sun_path) - 1;

UnixAddress::UnixAddress(){
	memset(&m_addr,0 ,sizeof(m_addr));
	m_addr.sun_family = AF_UNIX;
	m_length = offsetof(sockaddr_un, sun_path) + MAX_PATH_LEN;
	
}
UnixAddress::UnixAddress(const std::string& path){
	memset(&m_addr,0,sizeof(m_addr));
	m_addr.sun_family = AF_UNIX;
	m_length = path.size() +1;

	if(!path.empty() && path[0] == '\0'){
		--m_length;
	}

	if(m_length > sizeof(m_addr.sun_path)){
		throw std::logic_error("lenth too long");
	}

	memcpy(&m_addr.sun_path,path.c_str() ,m_length);
	m_length += offsetof(sockaddr_un, sun_path);
}

sockaddr* UnixAddress::getAddr()const{
	return (sockaddr*)&m_addr;
}
socklen_t UnixAddress::getAddrLen() const{
	return m_length;
}
std::ostream& UnixAddress::insert(std::ostream& os)const{
	if(m_length > offsetof(sockaddr_un, sun_path)
	  &&  m_addr.sun_path[0] == '\0' ){
		return os << "\\0" << std::string(m_addr.sun_path + 1,
				m_length - offsetof(sockaddr_un, sun_path) - 1);
	}
	return os << m_addr.sun_path;

}

UnknowAddress::UnknowAddress(int family){
	memset(&m_addr, 0,sizeof(m_addr));
	m_addr.sa_family = family;
}

sockaddr* UnknowAddress::getAddr()const{
	return (sockaddr*)&m_addr;
}
socklen_t UnknowAddress::getAddrLen() const {
	return (socklen_t)sizeof(m_addr);
}
std::ostream& UnknowAddress::insert(std::ostream& os)const{
	os << "[UnknowAddress]" <<" family:"<<m_addr.sa_family;
	return os;
}


}
