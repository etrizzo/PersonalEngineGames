#pragma once
#include "Engine/Networking/NetAddress.hpp"

typedef void* socket_t;

//------------------------------------------------------------------------
// TCPSocket Helper
class TCPSocket 
{
public:
	TCPSocket(); 
	TCPSocket(socket_t socket, NetAddress addr);
	~TCPSocket(); 

	// - - - - - -
	// Options
	// - - - - - -
	bool SetUnblocking();
	bool SetBlocking();

	// - - - - - -
	// STARTING/STOPPING
	// - - - - - -
	bool Listen( std::string port, unsigned int max_queued = 16); 
	TCPSocket* Accept(); 
	

	// for joining
	bool Connect( NetAddress const &addr ); 

	// when finished
	void Close(); 

	// - - - - - -
	// TRAFFIC
	// - - - - - -
	// returns how much sent
	size_t Send( void const *data, size_t const data_byte_size ); 
	// returns how much received
	size_t Receive( void *buffer, size_t const max_byte_size ); 

	// - - - - - -
	// HELPERS
	// - - - - - -
	bool IsClosed() const { return !m_isOpen; }; 
	std::string ToString() const;
	bool HasFatalError();

public:
	socket_t m_handle = nullptr; 

	// if you're a listening, the address is YOUR address
	// if you are connecting (or socket is from an accept)
	// this address is THEIR address;  (do not listen AND connect on the same socket)
	NetAddress m_address;
	bool m_isOpen = false;
};
