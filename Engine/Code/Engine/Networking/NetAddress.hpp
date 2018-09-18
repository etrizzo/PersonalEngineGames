#pragma once
#include "Engine/Core/LogSystem.hpp"
struct sockaddr;


struct NetAddress
{
public:
	NetAddress() {};
	NetAddress( sockaddr const *addr ); 
	NetAddress( std::string addrstr );		//takes "host:port" and constructs sockaddr

	bool ToSockAddr( sockaddr *out, size_t *out_addrlen ) const; 
	bool FromSockAddr( sockaddr const *sa ); 

	bool IsValid() const;

	std::string ToString() const; 

public:
	unsigned int m_ip4address	= 0;		//what should these be??
	uint16_t m_port				= 0; 

public: // STATICS
	static NetAddress GetLocal(char const* port = "80"); 
	static bool GetBindableAddress(NetAddress* out, std::string port);

	/* these are optional, but useful helpers
	static uint GetAllForHost( net_address_t *out, 
	uint max_count, 
	char const *host_name, 
	char const *service_name ); 
	static uint GetAllLocal( net_address_t *out, uint max_count ); */
};


void NetAddressToSocketAddress( sockaddr* outSockAddr, size_t& outSockAddrLen, NetAddress addr );