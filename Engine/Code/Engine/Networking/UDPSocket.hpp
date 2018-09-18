#pragma once
#include "Engine/Networking/Socket.hpp"

// net/udpsocket.hpp

// when searching for addresses - you are no longer looking for AF_INET
// you look for 

class UDPSocket : public Socket
{
public: 
	//
	bool Bind( NetAddress const &addr, // address I want to listen for traffic on
		uint16_t port_range = 0U );		  // how many additional ports to bind on (so you bind from [addr.port,addr.port + port_range])

										  // return 0 on failure, otherwise how large is the next datagram
										  // TODO in class
	size_t SendTo( NetAddress const &addr, void const *data, size_t const byte_count );	
	size_t ReceiveFrom( NetAddress *out_addr, void *buffer, size_t const max_read_size );
	
	bool HasFatalError();
};

