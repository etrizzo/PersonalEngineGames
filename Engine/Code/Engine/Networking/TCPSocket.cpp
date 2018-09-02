#include "Engine/Networking/TCPSocket.hpp"
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Networking/Net.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"

TCPSocket::TCPSocket()
{
	// now we have an address, we can try to bind it; 
	// first, we create a socket like we did before; 
	SOCKET sock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); 
	m_handle = (void*) sock;
	m_address;
}

TCPSocket::TCPSocket(socket_t socket, NetAddress addr)
{
	m_handle = socket;
	m_address = addr;
}

TCPSocket::~TCPSocket()
{
}

bool TCPSocket::Listen(std::string port, unsigned int max_queued)
{
	
	//std::string portstring = (char *) port;
	// Bindable means - use AI_PASSIVE flag
	if (!NetAddress::GetBindableAddress( &m_address, port)) {
		Close();
		return false; 
	}

	// Next, we bind it - which means we assign an address to it; 
	sockaddr_storage saddr;
	size_t addrlen; 
	m_address.ToSockAddr( (sockaddr*)&saddr, &addrlen ); 

	int result = ::bind( (SOCKET) m_handle, (sockaddr*)&saddr, addrlen ); 
	if (result == SOCKET_ERROR) {
		// failed to bind - if you want to know why, call WSAGetLastError()
		Close();
		return false; 
	}

	// we now have a bound socket - this means we can start listening on it; 
	// This allows the socket to queue up connections - like opening a call center.
	// The number passed is NOT the max number of connections - just the max number of people who can 
	// be waiting to connect at once (think of it has a call center with N people manning the phone, but many more 
	// people who calls may be forwarded to.  Once a call is forwarded, the person answering calls can now answer a new one)
	result = ::listen( (SOCKET) m_handle, max_queued ); 
	if (result == SOCKET_ERROR) {
		Close();
		return false; 
	}
	ConsolePrintf("Started hosting on: %s", m_address.ToString().c_str());
	LogTaggedPrintf("net", "Started hosting on: %s", m_address.ToString().c_str());
	m_isOpen = true;
}

TCPSocket * TCPSocket::Accept()
{
	sockaddr_storage their_addr; 
	int their_addrlen = sizeof(sockaddr_storage); 
	SOCKET their_sock = ::accept( (SOCKET) m_handle, (sockaddr*)&their_addr, &their_addrlen ); 
	TCPSocket* them = new TCPSocket((socket_t) their_sock, NetAddress((sockaddr*) &their_addr));
	return them;
}

bool TCPSocket::Connect(NetAddress const & addr)
{
	sockaddr_storage saddr;
	size_t addrlen;
	addr.ToSockAddr((sockaddr*) &saddr, &addrlen);

	int result = ::connect( (SOCKET) m_handle, (sockaddr*)&saddr, (int)addrlen ); 
	if (result == SOCKET_ERROR) {
		ConsolePrintf(RGBA::RED, "Could not connect" ); 
		Close();
		return false; 
	}

	ConsolePrintf(RGBA::YELLOW, "Connected." ); 
	m_isOpen = true;
	return true;
}

void TCPSocket::Close()
{
	::closesocket((SOCKET) m_handle); 
	m_isOpen = false;
}

size_t TCPSocket::Send(void const * data, size_t const data_byte_size)
{
	int sent = ::send( (SOCKET) m_handle, (char*) data, data_byte_size, MSG_OOB ); 
	if (sent == SOCKET_ERROR) {
		// there are non-fatal errors - but we'll go over them 
		// on Monday.  For now, you can assume any error with blocking
		// is a disconnect; 
		ConsolePrintf(RGBA::RED, "something fucked up :)");
		Close(); 
		return 0U; 
	} else { 
		return sent;
	}
}

size_t TCPSocket::Receive(void * buffer, size_t const max_byte_size)
{
	int recvd = ::recv( (SOCKET) m_handle, 
		(char*) buffer,           // what we read into
		max_byte_size,         // max we can read
		0U );             // flags (unused)
	return (size_t) recvd;
}
