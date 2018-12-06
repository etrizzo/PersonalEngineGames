#include "Socket.hpp"
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/DevConsole.hpp"

Socket::Socket()
{
	// now we have an address, we can try to bind it; 
	// first, we create a socket like we did before; 
	SOCKET sock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); 
	m_handle = (void*) sock;
	m_address;
}

Socket::~Socket()
{
	Close();
}

bool Socket::SetUnblocking()
{
	u_long nonblocking = 1;
	::ioctlsocket( (SOCKET)m_handle, FIONBIO, &nonblocking );
	return true;
}

bool Socket::SetBlocking()
{
	u_long blocking = 0;
	::ioctlsocket( (SOCKET)m_handle, FIONBIO, &blocking );
	return true;
}

bool Socket::Close()
{
	int closed = ::closesocket((SOCKET) m_handle); 
	m_isOpen = false;
	return (closed != -1);
}

bool Socket::IsClosed() const
{
	return !m_isOpen;
}

NetAddress const & Socket::GetAddress() const
{
	return m_address;
}

std::string Socket::GetAddressAsString() const
{
	return m_address.ToString();
}

bool IsFatalSocketError(int errorCode)
{
	if (errorCode == WSAEWOULDBLOCK || errorCode == WSAEMSGSIZE || errorCode == 0 || errorCode == WSAECONNRESET){
		return false;
	} else {
		ConsolePrintf("Fatal error: [%i]", errorCode);
	}
	return true;
}
