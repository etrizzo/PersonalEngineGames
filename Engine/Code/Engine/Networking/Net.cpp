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
void ConnectExampleWinSock( std::string addr_str, std::string msg)
{

	// first, create the socket - this allows us to setup options; 
	SOCKET sock = socket( AF_INET,   // address (socket) family (IPv4)
		SOCK_STREAM,                  // type (TCP is a stream based packet)
		IPPROTO_TCP );                // protocol, TCP

	if (sock == INVALID_SOCKET) {
		ConsolePrintf(RGBA::RED, "Could not create socket." );
		return; 
	}


	NetAddress address = NetAddress(addr_str);
	// CONFIRM asserts in debug if it evaluates to false, and release just runs the inside code;

	sockaddr_storage saddr;
	size_t addrlen;
	address.ToSockAddr((sockaddr*) &saddr, &addrlen);

	// we then try to connect.  This will block until it either succeeds
	// or fails (which possibly requires a timeout).  We will talk about making
	// this non-blocking later so it doesn't hitch the game (more important 
	// when connecting to remote hosts)
	int result = ::connect( sock, (sockaddr*)&saddr, (int)addrlen ); 
	if (result == SOCKET_ERROR) {
		ConsolePrintf(RGBA::RED, "Could not connect to %s (error: %u)", addr_str.c_str(), result ); 
		::closesocket(sock);  // frees the socket resource; 
		return; 
	}

	ConsolePrintf(RGBA::YELLOW, "Connected." ); 

	// you send raw bytes over the network in whatever format you want; 
	char payload[256]; 
	::send( sock, msg.c_str(), msg.size(), 0 ); 

	// with TCP/IP, data sent together is not guaranteed to arrive together.  
	// so make sure you check the return value.  This will return SOCKET_ERROR
	// if the host disconnected, or if we're non-blocking and no data is there. 
	size_t recvd = ::recv( sock, payload, 256 - 1U, 0 ); 

	// it may not be null terminated and I'm printing it, so just for safety. 
	payload[recvd] = NULL; 
	ConsolePrintf(RGBA::YELLOW, "Recieved: %s", payload ); 

	// cleanup
	::closesocket(sock); 
}

void ConnectExample(std::string addr_str, std::string msg)
{
	NetAddress addr = NetAddress(addr_str);

	TCPSocket socket; // defaults to blocking 
	if (socket.Connect( addr )) {
		ConsolePrintf(RGBA::YELLOW, "Connected." ); 
		socket.Send( msg.c_str(), msg.size() + 1 ); 

		char payload[256]; 
		size_t recvd = socket.Receive( payload, 256 ); 
		payload[recvd] = NULL;
		ConsolePrintf(RGBA::YELLOW, "Recieved: %s", payload ); 

		socket.Close(); 
	} else {
		ConsolePrintf(RGBA::RED, "Could not connect." );
	}

}


//------------------------------------------------------------------------
// Start with Connecting - with direct calls - something that'll look like this; 
//------------------------------------------------------------------------
//------------------------------------------------------------------------
// Listen (Host) Example
bool HostExampleWinSock( void* port )
{
	NetAddress addr;
	std::string portstring = (char *) port;
	// Bindable means - use AI_PASSIVE flag
	if (!NetAddress::GetBindableAddress( &addr, portstring)) {
		return false; 
	}

	// now we have an address, we can try to bind it; 
	// first, we create a socket like we did before; 
	SOCKET sock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); 

	// Next, we bind it - which means we assign an address to it; 
	sockaddr_storage saddr;
	size_t addrlen; 
	addr.ToSockAddr( (sockaddr*)&saddr, &addrlen ); 

	int result = ::bind( sock, (sockaddr*)&saddr, addrlen ); 
	if (result == SOCKET_ERROR) {
		// failed to bind - if you want to know why, call WSAGetLastError()
		::closesocket(sock); 
		return false; 
	}

	// we now have a bound socket - this means we can start listening on it; 
	// This allows the socket to queue up connections - like opening a call center.
	// The number passed is NOT the max number of connections - just the max number of people who can 
	// be waiting to connect at once (think of it has a call center with N people manning the phone, but many more 
	// people who calls may be forwarded to.  Once a call is forwarded, the person answering calls can now answer a new one)
	int max_queued = 16;  // probably pick a number that is <= max number of players in your game. 
	result = ::listen( sock, max_queued ); 
	if (result == SOCKET_ERROR) {
		::closesocket(sock); 
		return false; 
	}
	ConsolePrintf("Started hosting on: %s", addr.ToString().c_str());
	LogTaggedPrintf("net", "Started hosting on: %s", addr.ToString().c_str());

	// now we can "accept" calls. 
	bool is_running = true; 
	while (is_running) {
		// this will create a new socket
		// and fill in the address associated with that socket (who are you talking to?)
		sockaddr_storage their_addr; 
		int their_addrlen = sizeof(their_addr); 
		SOCKET their_sock = ::accept( sock, (sockaddr*)&their_addr, &their_addrlen ); 

		if (sock != INVALID_SOCKET) {
			// great, we have a socket - we can now send and receive on it just as before
			// with this test - we'll always listen, then send, but 
			byte_t buffer[1 * KB]; 

			int recvd = ::recv( their_sock, 
				(char*) buffer,           // what we read into
				256 - 1U,         // max we can read
				0U );             // flags (unused)

			if (recvd != SOCKET_ERROR) {
				buffer[recvd] = NULL; // just cause I'm printing it
				ConsolePrintf(RGBA::YELLOW, "Received: %s", buffer ); 

				// me sending a response, the word PONG which is 5 long (when you include the null terminator)
				::send( their_sock, "yote", 4, 0U ); 
			}
			// clean up after you're done; 
			::closesocket(their_sock); 
		}

		// something should tell me to stop servicing requests eventually
		// is_running = false; 
	}

	// and I'm done; 
	::closesocket(sock); 
	return true;
}

bool HostExample(void * port)
{
	// NEW ==--
	std::string portstring = (char *) port;
	int max_queued = 16;  // probably pick a number that is <= max number of players in your game. 
	TCPSocket host; 
	if (!host.Listen( portstring, max_queued )) {
		ConsolePrintf(RGBA::RED, "Cannot listen on port %s", portstring.c_str() );
		return false; 
	} 

	while (!host.IsClosed()) {
		TCPSocket *them = host.Accept();
		if (them != nullptr) {
			byte_t buffer[1 * KB]; 
			unsigned int received = them->Receive( buffer, (1 * KB) - 1U ); 
			if (received > 0U) {
				buffer[received] = NULL;
				ConsolePrintf(RGBA::YELLOW, "Receieved: %s", buffer ); 

				them->Send( "Thank", 6 ); 
			}

			delete them; 
		}
	}

	host.Close(); 
	return true;
	// END NEW-- 
}



