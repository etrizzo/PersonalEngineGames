#pragma once
#include "Engine/Networking/TCPSocket.hpp"

struct sockaddr;




class Net
{
public:
	static bool Startup();
	static bool Shutdown();

};

void LogIP( sockaddr_in* in);

bool GetAddressForHost( sockaddr * out, int * out_addrlen, char const* hostname, char const* service = "12345");
void GetAddressExample();

void ConnectExampleWinSock( std::string addr_str, std::string msg);
void ConnectExample( std::string addr_str, std::string msg);
bool HostExampleWinSock( void* port );
bool HostExample(void* port);