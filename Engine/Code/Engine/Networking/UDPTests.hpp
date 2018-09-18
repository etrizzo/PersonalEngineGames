#include "Engine/Networking/UDPSocket.hpp"

// class test
#define GAME_PORT "10084"

//ETHERNET_MTU is the largest amount of data we can send.
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it; 
#define PACKET_MTU (ETHERNET_MTU - 40 - 8) 

// IPv4 Header Size: 20B
// IPv6 Header Size: 40B
// TCP Headre Size: 20B-60B
// UDP Header Size: 8B 
// Ethernet: 28B, but MTU is already adjusted for it
// so packet size is 1500 - 40 - 8 => 1452B (why?)

class UDPTest
{
public:
	UDPTest();

	bool Start();
	

	void Stop();

	void SendTo( NetAddress const &addr, void const *buffer, unsigned int byte_count );
	

	void Update();
	

public:
	// if you have multiple address, you can use multiple sockets
	// but you have to be clear on which one you're sending from; 
	UDPSocket m_socket; 
	bool m_isRunning = false;
};
