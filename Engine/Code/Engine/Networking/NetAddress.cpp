#include "Engine/Networking/NetAddress.hpp"
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Networking/Net.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"


NetAddress::NetAddress(sockaddr const * addr)
{
	if (!FromSockAddr(addr)){
		ConsolePrintf(RGBA::RED, "Could not resolve socket.");
		m_ip4address = 0;
		m_port = 0;

	}
}

NetAddress::NetAddress(std::string addrstr)
{
	if (addrstr.size() == 0 ){
		ConsolePrintf(RGBA::RED, "Must provide an address and a message." ); 
		return; 
	}

	Strings addrbits; 
	Split(addrstr, ':', addrbits);
	std::string ip = addrbits[0];
	std::string port = addrbits[1];

	// sockaddr storage is a sockaddr struct that
	// is large enough to fit any other sockaddr struct
	// sizeof(sockaddr_storage) >= sizeof(any other sockaddr)
	sockaddr_storage saddr;
	int addrlen; 
	if (!GetAddressForHost( (sockaddr*)&saddr, &addrlen, ip.c_str(), port.c_str() )) {
		ConsolePrintf(RGBA::RED, "Could not resolve host.");
		return;
	}

	FromSockAddr((sockaddr*) &saddr);
}

bool NetAddress::ToSockAddr(sockaddr * out, size_t * out_addrlen) const
{
	*out_addrlen = sizeof(sockaddr_in); 

	sockaddr_in *ipv4 = (sockaddr_in*) out; 
	memset( ipv4, 0, sizeof(sockaddr_in) ); 

	ipv4->sin_family = AF_INET;
	ipv4->sin_addr.S_un.S_addr = m_ip4address; 
	ipv4->sin_port = ::htons(m_port); 
	return true; 
}

bool NetAddress::FromSockAddr(sockaddr const * sa)
{
	if (sa->sa_family != AF_INET) {
		return false; 
	}
	// if IPv6 - also check AF_INET6

	sockaddr_in const *ipv4 = (sockaddr_in const*) sa; 

	unsigned int ip = ipv4->sin_addr.S_un.S_addr; 
	unsigned int port = ::ntohs( ipv4->sin_port );

	m_ip4address = ip;
	m_port = (uint16_t) port; 
	return true; 
}

std::string NetAddress::ToString() const
{
	// tostring
	uint8_t *array = (uint8_t*)&m_ip4address; 
	std::string s = Stringf( "%u.%u.%u.%u:%u", 
		array[0], 
		array[1], 
		array[2], 
		array[3], 
		m_port ); 

	return s;
}

NetAddress NetAddress::GetLocal(char const* port)
{
	char my_name[256];
	if (SOCKET_ERROR == ::gethostname (my_name, 256)) {		//gethostname gets YOUR machine name
		ConsolePrintf(RGBA::RED, "GetHostName failed  - did you startup?? :(");
		return NetAddress();
	}

	//char const* service = "80"; //service is like "http" or "ftp" which translates to a port (80 or 21).
								//we will just use port 80 for this example (tends to be http).

								//if there is no host name, can't resolve
	if (my_name[0] == '\0' || my_name == NULL){
		ConsolePrintf(RGBA::RED, "No name :(");
		return NetAddress();
	}

	// there are many ways to communicate w/ the machine
	// and many addresses are associated with it, so we need to provide a hint
	// to the API to filter down to only the address we carea bout
	addrinfo hints;
	memset(&hints, 0, sizeof(hints) ); //initialized to all zero

	hints.ai_family = AF_INET;	//IPv4 addresses
	hints.ai_socktype = SOCK_STREAM;	//TCP socket (SOCK_DGRAM for UDP)
	hints.ai_flags = AI_PASSIVE;	//says this is an address we can host on;
									// hints.ai_flags |= FINDADDR_NUMERIC_ONLY		//will speed up this function later bc we won't have to look up addr


	addrinfo *result = nullptr;
	int status = ::getaddrinfo(my_name, port, &hints, &result );		//goes until next us null
	if (status != 0) {
		LogErrorf( "net", "Failed to find addresses for [%s:%s].  Error[%s]", 
			my_name, port, ::gai_strerror(status) );
		return NetAddress(); 
	}

	// result now is a linked list of addresses that match our filter; 
	addrinfo *iter = result;
	while (iter != nullptr) {
		// you can farther filter here if you want, or return all of them and try them in order; 
		// for example, if you're using VPN, you'll get two unique addresses for yourself; 
		// if you're using AF_INET, the address is a sockaddr_in; 
		if (iter->ai_family == AF_INET) {
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr);
			//we have an address now, so we print it

			// if you look at the bytes - you can "see" the address, but backwards... we'll get to that
			// (port too)
			char out[256]; 
			//converts the address to a human-readable string
			inet_ntop( ipv4->sin_family, &(ipv4->sin_addr), out, 256 ); 
			//LogTaggedPrintf( "net", "My Address: %s", out ); 
			NetAddress address = NetAddress((sockaddr*) ipv4);
			::freeaddrinfo( result );
			return address;
		}
		iter = iter->ai_next;
	}
	//free up result
	::freeaddrinfo( result );
	return NetAddress();

}

bool NetAddress::GetBindableAddress(NetAddress * out,  std::string port)
{
	char my_name[256];
	if (SOCKET_ERROR == ::gethostname (my_name, 256)) {		//gethostname gets YOUR machine name
		ConsolePrintf(RGBA::RED, "GetHostName failed  - did you startup?? :(");
		return false;
	}

	//char const* service = "80"; //service is like "http" or "ftp" which translates to a port (80 or 21).
								//we will just use port 80 for this example (tends to be http).

								//if there is no host name, can't resolve
	if (my_name[0] == '\0' || my_name == NULL){
		ConsolePrintf(RGBA::RED, "No name :(");
		return false;
	}

	// there are many ways to communicate w/ the machine
	// and many addresses are associated with it, so we need to provide a hint
	// to the API to filter down to only the address we carea bout
	addrinfo hints;
	memset(&hints, 0, sizeof(hints) ); //initialized to all zero

	hints.ai_family = AF_INET;	//IPv4 addresses
	hints.ai_socktype = SOCK_STREAM;	//TCP socket (SOCK_DGRAM for UDP)
	hints.ai_flags = AI_PASSIVE;	//says this is an address we can host on;
									// hints.ai_flags |= FINDADDR_NUMERIC_ONLY		//will speed up this function later bc we won't have to look up addr


	addrinfo *result = nullptr;
	int status = ::getaddrinfo(my_name, port.c_str(), &hints, &result );		//goes until next us null
	if (status != 0) {
		LogErrorf( "net", "Failed to find addresses for [%s:%s].  Error[%s]", 
			my_name, port.c_str(), ::gai_strerror(status) );
		return false;
	}

	// result now is a linked list of addresses that match our filter; 
	addrinfo *iter = result;
	while (iter != nullptr) {
		// you can farther filter here if you want, or return all of them and try them in order; 
		// for example, if you're using VPN, you'll get two unique addresses for yourself; 
		// if you're using AF_INET, the address is a sockaddr_in; 
		if (iter->ai_family == AF_INET) {
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr);
			//we have an address now, so we print it

			// if you look at the bytes - you can "see" the address, but backwards... we'll get to that
			// (port too)
			char outaddress[256]; 
			//converts the address to a human-readable string
			inet_ntop( ipv4->sin_family, &(ipv4->sin_addr), outaddress, 256 ); 
			//LogTaggedPrintf( "net", "My Address: %s", out ); 
			*out = NetAddress((sockaddr*) ipv4);
			::freeaddrinfo( result );
			return true;
		}
		iter = iter->ai_next;
	}


	//free up result
	::freeaddrinfo( result );
	return false;
}


