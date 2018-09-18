#include "UDPTests.hpp"
#include "Engine/Core/DevConsole.hpp"

UDPTest::UDPTest()
{
	m_socket = UDPSocket();
}

bool UDPTest::Start()
{
	// get an address to use; 
	NetAddress addr;
	addr = NetAddress::GetLocal(GAME_PORT);//NetAddressesForMe( &addr, 1, GAME_PORT );  
	if (!addr.IsValid()) {
		ConsolePrintf( "No addresses" ); 
		return false;
	}

	if (!m_socket.Bind( addr, 10 )) {
		ConsolePrintf( "Failed to bind." );
		return false;  
	} else {
		m_socket.SetUnblocking(); // if you have cached options, you could do this
									  // in the constructor; 
		ConsolePrintf( "Socket bound: %s", m_socket.GetAddress().ToString().c_str() );
		return true; 
	}
	m_isRunning = true;
}

void UDPTest::Stop()
{
	m_socket.Close(); 
	m_isRunning = false;
}

void UDPTest::SendTo(NetAddress const & addr, void const * buffer, unsigned int byte_count)
{
	m_socket.SendTo( addr, buffer, byte_count ); 
}

void UDPTest::Update()
{
	//if (m_isRunning){
		byte_t buffer[PACKET_MTU]; 

		NetAddress from_addr; 

		size_t read = m_socket.ReceiveFrom( &from_addr, buffer, PACKET_MTU ); 


		//processing message
		if (read > 0U) {
			unsigned int max_bytes = Min(read, (size_t) 128);
			unsigned int string_size = max_bytes * 2U + 3U;
			char* outbuffer = new char[string_size];
			sprintf_s (outbuffer, 3U, "0x");
			byte_t *iter = (byte_t*) outbuffer;
			iter += 2U;
			for (unsigned int i = 0; i <read; ++i){
				sprintf_s((char*) iter, 3U, "%02X", buffer[i]);
				iter+=2U;
			}
			*iter = NULL;

			ConsolePrintf( "Received from %s;\n%s", from_addr.ToString().c_str(), outbuffer );
			delete[] outbuffer;
		}
	//}
}
