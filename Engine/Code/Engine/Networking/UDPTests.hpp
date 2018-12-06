#include "Engine/Networking/UDPSocket.hpp"





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
