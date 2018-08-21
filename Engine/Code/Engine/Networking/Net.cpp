#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Networking/Net.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"

bool Net::Startup()
{
	//pick the version we want
	WORD version = MAKEWORD(2,2);
	
	// initialize the winsockAPI (WSA)
	// data will store info about your system and some of its limitations
	// (if only using a few sockets, it's not important)
	WSADATA data;
	int error = ::WSAStartup( version, &data);

	//return whether we actually started up
	ASSERT_OR_DIE( error == 0, "Network startup failed");
	return (error == 0);

	//Note: For linux, this function just returns true, because sockets are enabled by default
}

bool Net::Shutdown()
{
	//not strictly necessarily, but do it
	::WSACleanup();
	return true;
}

void LogIP(sockaddr_in * in)
{

	// if you look at the bytes - you can "see" the address, but backwards... we'll get to that
	// (port too)
	char out[256]; 
	//converts the address to a human-readable string
	inet_ntop( in->sin_family, &(in->sin_addr), out, 256 ); 
	LogTaggedPrintf( "net", "Address: %s", out ); 
}

bool GetAddressForHost(sockaddr * out, int * out_addrlen, char const * hostname, char const * service)
{
	//if there is no host name, can't resolve
	if (hostname[0] == '\0' || hostname == NULL){
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
	//hints.ai_flags = AI_PASSIVE;	//says this is an address we can host on;
									// hints.ai_flags |= FINDADDR_NUMERIC_ONLY		//will speed up this function later bc we won't have to look up addr


	addrinfo *result = nullptr;
	int status = ::getaddrinfo(hostname, service, &hints, &result );		//goes until next us null
	if (status != 0) {
		LogTaggedPrintf( "net", "Failed to find addresses for [%s:%s].  Error[%s]", 
			hostname, service, ::gai_strerror(status) );
		return false; 
	}
	bool found = false;
	// result now is a linked list of addresses that match our filter; 
	addrinfo *iter = result;
	while (iter != nullptr) {
		// you can farther filter here if you want, or return all of them and try them in order; 
		// for example, if you're using VPN, you'll get two unique addresses for yourself; 
		// if you're using AF_INET, the address is a sockaddr_in; 
		if (iter->ai_family == AF_INET) {
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr);
			//we have an address now, so we print it

			memcpy(out, ipv4, sizeof(sockaddr_in));
			*out_addrlen = sizeof(sockaddr_in);
			found = true;
			break;
		}
		iter = iter->ai_next;
	}


	//free up result
	// (this is safer than just using free() because we don't really know how they allocate)
	::freeaddrinfo( result );
	return found;
}

void GetAddressExample()
{

	char my_name[256];
	if (SOCKET_ERROR == ::gethostname (my_name, 256)) {		//gethostname gets YOUR machine name
		ConsolePrintf(RGBA::RED, "GetHostName failed  - did you startup?? :(");
		return;
	}

	char const* service = "80"; //service is like "http" or "ftp" which translates to a port (80 or 21).
	//we will just use port 80 for this example (tends to be http).

	//if there is no host name, can't resolve
	if (my_name[0] == '\0' || my_name == NULL){
		ConsolePrintf(RGBA::RED, "No name :(");
		return;
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
	int status = ::getaddrinfo(my_name, service, &hints, &result );		//goes until next us null
	if (status != 0) {
		LogTaggedPrintf( "net", "Failed to find addresses for [%s:%s].  Error[%s]", 
			my_name, service, ::gai_strerror(status) );
		return; 
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
			LogTaggedPrintf( "net", "My Address: %s", out ); 
		}
		iter = iter->ai_next;
	}


	//free up result
	::freeaddrinfo( result );
}

//------------------------------------------------------------------------
// Connect/Send/Recv Example
void ConnectExample( net_address_t const &addr, char const *msg )
{
	//// first, create the socket - this allows us to setup options; 
	//SOCKET sock = socket( AF_INET,   // address (socket) family (IPv4)
	//	SOCK_STREAM,                  // type (TCP is a stream based packet)
	//	IPPROTO_TCP );                // protocol, TCP

	//if (sock == INVALID_SOCKET) {
	//	ConsolePrintf(RGBA::RED, "Could not create socket." );
	//	return; 
	//}

	//// sockaddr storage is a sockaddr struct that
	//// is large enough to fit any other sockaddr struct
	//// sizeof(sockaddr_storage) >= sizeof(any other sockaddr)
	//sockaddr_storage saddr;
	//int addrlen; 
	//CONFIRM( addr.to_sockaddr( (sockaddr*) &saddr, &addrlen ) ); 
	//// CONFIRM asserts in debug if it evaluates to false, and release just runs the inside code;

	//// we then try to connect.  This will block until it either succeeds
	//// or fails (which possibly requires a timeout).  We will talk about making
	//// this non-blocking later so it doesn't hitch the game (more important 
	//// when connecting to remote hosts)
	//int result = ::connect( sock, (sockaddr*)&saddr, (int)addrlen ); 
	//if (result == SOCKET_ERROR) {
	//	ConsolePrintf(RGBA::RED, "Could not connect" ); 
	//	::closesocket(sock);  // frees the socket resource; 
	//	return; 
	//}

	//ConsolePrintf(RGBA::YELLOW, "Connected." ); 

	//// you send raw bytes over the network in whatever format you want; 
	//char payload[256]; 
	//::send( sock, msg, (int)StringGetSize(msg), 0 ); 

	//// with TCP/IP, data sent together is not guaranteed to arrive together.  
	//// so make sure you check the return value.  This will return SOCKET_ERROR
	//// if the host disconnected, or if we're non-blocking and no data is there. 
	//size_t recvd = ::recv( sock, payload, 256 - 1U, 0 ); 

	//// it may not be null terminated and I'm printing it, so just for safety. 
	//payload[recvd] = NULL; 
	//ConsolePrintf(RGBA::YELLOW, "Recieved: %s", payload ); 

	//// cleanup
	//::closesocket(sock); 
}
