#include "NetConnection.hpp"
#include "Engine/Networking/NetPacket.hpp"
#include "Engine/Networking/NetSession.hpp"



NetConnection::NetConnection(NetSession * owningSession, uint8_t indexInSession, NetAddress addr)
{
	m_owningSession = owningSession;
	m_indexInSession = indexInSession;
	m_address = addr;
}

void NetConnection::ClearMessageQueue()
{
	m_messagesToSend.clear();
}

void NetConnection::Send(NetMessage * msg)
{
	msg->SetDefinition(m_owningSession->GetRegisteredMessageByName(msg->m_msgName));
	msg->WriteHeader();
	m_messagesToSend.push_back(msg);
}

NetAddress NetConnection::GetAddress() const
{
	return m_address;
}
