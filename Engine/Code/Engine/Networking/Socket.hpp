#pragma once
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Networking/NetAddress.hpp"

// net/socket.hpp
// this is just so it is a typed pointer - but
// internally we cast socket_t to SOCKET
typedef void* socket_t; 
//typedef void* socket_t

enum eSocketOptionBit : unsigned int 
{
	SOCKET_OPTION_BLOCKING = BIT_FLAG(0),

	// some other options that may be useful to support
	// SOCKET_OPTION_BROADCAST - socket can broadcast messages (if supported by network)
	// SOCKET_OPTION_LINGER - wait on close to make sure all data is sent
	// SOCKET_OPTION_NO_DELAY - disable nagle's algorithm
};
typedef unsigned int eSocketOptions; 

// Rip out things from TCPSocket and put it to a base leve; 
class Socket 
{
public:
	Socket(); 
	virtual ~Socket(); // closes

	// - - - - - -
	// Options
	// - - - - - -
	bool SetUnblocking();
	bool SetBlocking();


	bool Close(); // closesocket and set m_handle = INVALID_SOCKET
	bool IsClosed() const; // m_handle == (socket_t)INVALID_SOCKET; 

	NetAddress const& GetAddress() const; 
	std::string GetAddressAsString()const;

protected:
	NetAddress m_address; // address assocated with this socket; 
	socket_t m_handle; // initialized to INVALID_SOCKET

					   // used if you want to set options while closed, 
					   // used to keep track so you can apply_options() after the socket
					   // gets created; 
	eSocketOptions m_options; 
	bool m_isOpen;
};

// fatal errors cause the socket to close; 
bool IsFatalSocketError( int errorCode ); 