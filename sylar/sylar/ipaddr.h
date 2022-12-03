#ifndef __SYLAR_IPADDR_H__
#define __SYLAR_IPADDR_H__

#include <bits/stdint-uintn.h>
#include <functional>
#include<sys/un.h>
#include<memory>
#include <ostream>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<map>
#include <vector>
namespace sylar{

class IPAddress;
class Address{

public:	
	typedef std::shared_ptr<Address> ptr;
	static bool GetInterfaceAddress(std::multimap<std::string,std::pair<Address::ptr,uint32_t> >& result,int family = AF_UNSPEC);
	
	static bool GetInterfaceAddress(std::vector<std::pair<Address::ptr,uint32_t> >& result,const std::string& iface,int family = AF_UNSPEC);

	static Address::ptr Create(const sockaddr* addr,socklen_t addrlen);
	
	static std::shared_ptr<IPAddress> LookupAnyIPAddress(const std::string& host,int family = AF_UNSPEC,int type = 0,int protocol = 0);
	static Address::ptr LookupAny(const std::string& host,int family = AF_UNSPEC,int type = 0,int protocol = 0);
	static bool Lookup(std::vector<Address::ptr>& vec,const std::string& host,int family = AF_UNSPEC,int type = 0,int protocol = 0);
	virtual ~Address(){}
	int getFamily()const;
	virtual sockaddr* getAddr()const = 0;
	virtual socklen_t getAddrLen() const = 0;
	virtual std::ostream& insert(std::ostream& os)const = 0;
	std::string toString();

	bool operator<(const Address& lhs) const;
	bool operator==(const Address& rhs) const;
	bool operator!=(const Address& rhs)const;


private:

};

class IPAddress:public Address{
public:
	typedef std::shared_ptr<IPAddress> ptr;
	virtual IPAddress::ptr broadcastAddress(uint32_t prefix_len) = 0;
	virtual IPAddress::ptr networkAddress(uint32_t prefix_len) = 0;
	virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;

	static IPAddress::ptr Create(const char* address, uint16_t port = 0);
	virtual uint32_t getPort()const  = 0;
	virtual void setPort(uint16_t v) = 0;



private:

};

class IPv4Address: public IPAddress{
public:

	 typedef std::shared_ptr<IPv4Address> ptr;
	 IPv4Address(uint32_t address = INADDR_ANY,uint16_t port = 0);
	 IPv4Address(const sockaddr_in& address);
	 
	 static IPv4Address::ptr Create(const char* address,uint16_t port);


	 sockaddr* getAddr()const override;
	 socklen_t getAddrLen() const override;
	 std::ostream& insert(std::ostream& os)const override;
	 IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
	 IPAddress::ptr networkAddress(uint32_t prefix_len) override;
	 IPAddress::ptr subnetMask(uint32_t prefix_len) override;

	 uint32_t getPort()const  override;
	 void setPort(uint16_t v) override;


private:
	 sockaddr_in m_addr;


};

class IPv6Address: public IPAddress{
public:
	 typedef std::shared_ptr<IPv6Address> ptr;
	 IPv6Address();
	 static IPv6Address::ptr Create(const char* address,uint16_t port = 0);
	 IPv6Address(const sockaddr_in6& address);
	 IPv6Address(const uint8_t address[16],uint16_t port = 0);
	 sockaddr* getAddr()const override;
	 socklen_t getAddrLen() const override;
	 std::ostream& insert(std::ostream& os)const override;
	 IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
	 IPAddress::ptr networkAddress(uint32_t prefix_len) override;
	 IPAddress::ptr subnetMask(uint32_t prefix_len) override;

	 uint32_t getPort()const  override;
	 void setPort(uint16_t v) override;
	


private:
	sockaddr_in6 m_addr;
};

class UnixAddress: public Address{
public:
	typedef std::shared_ptr<UnixAddress> ptr;
	UnixAddress(const std::string& path);
	UnixAddress();

	 sockaddr* getAddr()const override;
	 socklen_t getAddrLen() const override;
	 std::ostream& insert(std::ostream& os)const override;
	 void setAddrLen(socklen_t len){m_length = len;}


private:
	struct sockaddr_un m_addr;
	socklen_t m_length;
};

class UnknowAddress:public Address{
public:
	typedef std::shared_ptr<UnknowAddress> ptr;
	UnknowAddress(int family);
	UnknowAddress(const sockaddr addr);
	sockaddr* getAddr()const override;
	socklen_t getAddrLen() const override;
	std::ostream& insert(std::ostream& os)const override;
private:
	 sockaddr m_addr;
};

}


#endif
