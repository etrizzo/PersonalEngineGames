#include "Engine/Networking/UDPSocket.hpp"
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"

// cpp
//port_range is the range of ports you ~could~ bind if you can't get your first choice.
// i.e., if you try to bind to port 10, but it's taken, try port 11, 12, 13 ... until one succeeds or you hit port_range attempts
bool UDPSocket::Bind( NetAddress &addr, uint16_t port_range /*= 0U*/ )
{
	// create the socket. Maybe want to check if you have a socket already, but whatever
	SOCKET my_socket = socket( AF_INET,	// IPv4 to send...
		SOCK_DGRAM,							// ...Datagrams... 
		IPPROTO_UDP );						// ...using UDP.
			//NOte: DGRAM and UDP are tied together. Have to use DGRam for UDP

	if (my_socket == INVALID_SOCKET){
		return false;
	}

	for(uint16_t i = 0; i < port_range; i++){
		
		// TODO, try to bind all ports within the range.  
		// Shown - just trying one; 
		sockaddr_storage sock_addr;
		size_t sock_addr_len;
		addr.ToSockAddr((sockaddr*) &sock_addr, &sock_addr_len);
		//NetAddressToSocketAddress( (sockaddr*)&sock_addr, &sock_addr_len, addr );

		// try to bind - if it succeeds - great.  If not, try the next port in the range.
		//this is the standing on a table step. If you fail, you don't have a table to stand on.
		int result = ::bind( my_socket, (sockaddr*)&sock_addr, (int)sock_addr_len );
		if (0 == result) {
			m_handle = (socket_t)my_socket; 
			m_address = addr; 
			m_isOpen = true;
			return true; 
		} else {
			addr.IncrementPort();
		}
	}

	return false; 
}

size_t UDPSocket::SendTo(NetAddress const & addr, void const * data, size_t const byte_count)
{
	if (!m_isOpen){
		ConsolePrintf(RGBA::RED, "Socket is closed and cannot send.");
		return 0U;
	}
	//first, get a sockaddr
	sockaddr_storage sock_addr;
	size_t sock_addr_len;
	addr.ToSockAddr((sockaddr*) &sock_addr, &sock_addr_len);

	SOCKET sock = (SOCKET) m_handle;
	//send buffer from this socket (sock) TO sock_addr
	int sent = ::sendto(sock,		// socket we're sending from
		(char*) data,				// data we want to send
		(int) byte_count,			// bytes to send
		0 ,							// Unused flags
		(sockaddr*) &sock_addr,		// address we're sending to
		(int) sock_addr_len );

	if (sent > 0){
		if (sent != (int) byte_count){
			ConsolePrintf(RGBA::RED, "Didn't send entire message");
			return 0U;
		}
		return (size_t) sent;
	} else {
		if (HasFatalError()) {
			Close();
		}
		return 0U;
	}
}

size_t UDPSocket::ReceiveFrom(NetAddress * out_addr, void * buffer, size_t const max_read_size)
{
	if (!m_isOpen){
		return 0U;
	}

	sockaddr_storage fromaddr;	//who am i getting data from
	int addr_len = sizeof(sockaddr_storage);
	SOCKET sock = (SOCKET) m_handle;		//mmeeeeee

	int recvd = ::recvfrom( sock,		// what socket am I receiving on
		(char*) buffer,					// where i am receiving data to
		(int) max_read_size,			// max I can read
		0,								// unused flags
		(sockaddr*) &fromaddr,			// who sent it
		&addr_len );

	if (recvd > 0){
		//fill out addr
		out_addr->FromSockAddr((sockaddr*) &fromaddr);
		return recvd;
	} else {
		if (HasFatalError()){
			Close();
		}
		return 0U;
	}
}

bool UDPSocket::HasFatalError()
{
	int error = WSAGetLastError();
	if (IsFatalSocketError(error)){
		return true;
	}
	return false;
}


