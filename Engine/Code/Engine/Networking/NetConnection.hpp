#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Networking/UDPSocket.hpp"

class NetMessage;
class NetSession;

class NetConnection 
{
public:
	NetConnection(NetSession* owningSession, uint8_t indexInSession, NetAddress addr);
	std::vector<NetMessage*> m_messagesToSend;
	NetAddress m_address;
	uint8_t m_indexInSession;
	NetSession *m_owningSession; 

	void ClearMessageQueue();
	void Send(NetMessage* msg);

	NetAddress GetAddress() const;

};
