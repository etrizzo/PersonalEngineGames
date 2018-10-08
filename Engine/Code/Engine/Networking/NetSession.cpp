#include "NetSession.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Networking/NetConnection.hpp"
#include "Engine/Networking/UDPSocket.hpp"
#include "Engine/Networking/NetPacket.hpp"

NetSession::NetSession()
{
	m_socket = new UDPSocket();
}

void NetSession::Update()
{
	if (m_socket != nullptr){
		ProcessIncoming();
		ProcessOutgoing();
		ClearConnectionMesssages();
	}
}

void NetSession::AddBinding(const char * port)
{
	NetAddress addr = NetAddress::GetLocal(port);
	if (!m_socket->Bind(addr, MAX_CONNECTIONS)){
		ConsolePrintf(RGBA::RED, "Could not bind session to address: %s", port);
	} else {
		ConsolePrintf(RGBA::GREEN, "Hosting session on address %s", addr.ToString().c_str());
		m_socket->SetUnblocking();
	}
}

NetConnection * NetSession::AddConnection(uint8_t idx, NetAddress addr)
{
	NetConnection* newConnection = new NetConnection(this, idx, addr);
	m_connections[idx] = newConnection;
	return newConnection;
}

NetConnection * NetSession::GetConnection(uint8_t idx) const
{
	return m_connections[idx];
}

NetConnection * NetSession::GetConnectionByAddress(NetAddress addr) const
{
	for (int i = 0; i < MAX_CONNECTIONS; i++){
		if (m_connections[i] != nullptr){
			if (m_connections[i]->GetAddress() == addr){
				return m_connections[i];
			}
		}
	}
	return nullptr;
}

void NetSession::RegisterMessage(std::string name, NetSessionMessageCB messageCB)
{
	net_message_definition_t* messageDef = new net_message_definition_t();
	messageDef->m_messageCB = messageCB;
	messageDef->m_messageName = name;
	messageDef->m_messageID = (uint16_t) m_registeredMessages.size();
	m_registeredMessages.push_back(messageDef);
}

void NetSession::SortMessageIDs()
{
	std::sort(m_registeredMessages.begin(), m_registeredMessages.end(), CompareDefinitionsByName);
	for (int i = 0; i < (size_t) m_registeredMessages.size(); i++)
	{
		m_registeredMessages[i]->m_messageID = (uint16_t) i;
	}
}

bool NetSession::IsMessageRegistered(std::string name)
{
	for (int i = 0; i < m_registeredMessages.size(); i++){
		if (m_registeredMessages[i]->m_messageName == name){
			return true;
		}
	}
	return false;
}

bool NetSession::IsMessageRegistered(uint16_t id)
{
	for (int i = 0; i < m_registeredMessages.size(); i++){
		if (m_registeredMessages[i]->m_messageID == id){
			return true;
		}
	}
	return false;
}

net_message_definition_t* NetSession::GetRegisteredMessageByName(std::string name)
{
	for (int i = 0; i < m_registeredMessages.size(); i++){
		if (m_registeredMessages[i]->m_messageName == name){
			return m_registeredMessages[i];
		}
	}
	return nullptr;
}

net_message_definition_t * NetSession::GetRegisteredMessageByID(uint8_t id)
{
	for (int i = 0; i < m_registeredMessages.size(); i++){
		if (m_registeredMessages[i]->m_messageID == id){
			return m_registeredMessages[i];
		}
	}
	return nullptr;
}

void NetSession::ClearConnectionMesssages()
{
	for (NetConnection* connection : m_connections){
		if (connection != nullptr){
			connection->ClearMessageQueue();
		}
	}
}

void NetSession::ProcessIncoming()
{

	//see if you received anything
	byte_t buffer[PACKET_MTU]; 
	NetAddress from_addr; 
	size_t read = m_socket->ReceiveFrom( &from_addr, buffer, PACKET_MTU ); 
	
	
	//once you have a message, fill a BytePacker (NetMessage) like we did w/RCS (?)
	if (read > 0U){
		NetPacket packet = NetPacket();




		//NetMessage message;
		//get the size of the entire buffer with the same method we used for RCS
		packet.WriteBytes(read, buffer);		//writes the rest of the buffer to the NetMessage packer
	
		packet_header_t packetHeader;
		packet.ReadHeader(packetHeader);
		for (unsigned int i = 0; i < packetHeader.m_unreliableMessageCount; i++){
			NetMessage message;
			packet.ReadMessage(&message);
			ProcessMessage(message, from_addr);
		}
															//process the NetMessage
		
	}
}

void NetSession::ProcessOutgoing()
{
	// for every net conneciton, see if it has any messages to send
	// and send all messages queued during the frame using your socket
	// basically queueing all messages for a frame and then sending it?
	for (int i = 0; i < MAX_CONNECTIONS; i++){
		if (m_connections[i] != nullptr){
			NetPacket* packet = new NetPacket();
			packet->AdvanceWriteHead(sizeof(packet_header_t));
			uint8_t messagesSent = 0;
			for (int msgNum = 0; msgNum < m_connections[i]->m_messagesToSend.size(); msgNum++){
				NetMessage* msg = m_connections[i]->m_messagesToSend[msgNum];
				
				// write the message to the netpacket
				// if you dont' have the space to write a packet, send existing packet,
				// and start writing a new packet
				if (!packet->WriteMessage(*msg)){
					SendPacketToConnection(i, packet, messagesSent);
					messagesSent = 0;
					delete packet;
					packet = new NetPacket();
					packet->WriteMessage(*msg);		//this is always guaranteed to be smaller than net packet size
				}
				messagesSent++;
				//m_socket->SendTo(connection->GetAddress(), msg->GetBuffer(), msg->GetWrittenByteCount());
			}
			if (messagesSent > 0){
				SendPacketToConnection(i, packet, messagesSent);
			}
		}
	}
}

void NetSession::ProcessMessage(NetMessage message, NetAddress from)
{
	// figure out which kind of message it is
	// seems like the name is encoded as the first string?
	uint8_t messageType;
	message.Read(&messageType, false);

	//get the appropriate callback from the map of registered callbacks
	if (IsMessageRegistered(messageType)){
		// check to see if the sender is actually somebody connected to us? seems relevant?
		NetConnection* connection = GetConnectionByAddress(from);
		if (connection != nullptr){
			net_message_definition_t* definition = GetRegisteredMessageByID(messageType);
			((definition->m_messageCB))( message, net_sender_t(connection));
		}
	}
}

void NetSession::SendPacketToConnection(uint8_t connIndex, NetPacket * packet, uint8_t numMessages)
{
	packet_header_t packetHeader;
	packetHeader.m_unreliableMessageCount = numMessages;
	packetHeader.m_senderConnectionIndex = connIndex;
	packet->WriteHeader(packetHeader);
	m_socket->SendTo(m_connections[connIndex]->GetAddress(), packet->GetBuffer(), packet->GetWrittenByteCount());
}
	