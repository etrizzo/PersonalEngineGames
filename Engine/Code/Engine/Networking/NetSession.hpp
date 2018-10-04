#pragma once
#include "Engine/Networking/UDPSocket.hpp"
#include "Engine/Networking/NetMessage.hpp"

class NetPacket;
class NetConnection;
#define MAX_CONNECTIONS (16)




//Game has a global session...
class NetSession
{
public:
	NetSession();

	void Update();

	// This creates the socket(s) we can communicate on; 
	void AddBinding(const char* port);

	NetConnection* AddConnection(uint8_t idx, NetAddress addr);

	NetConnection* GetConnection(uint8_t idx) const;
	NetConnection* GetConnectionByAddress(NetAddress addr) const;

	//adds the callback to the map of possible messages this session can handle
	void RegisterMessage(std::string name, NetSessionMessageCB* messageCB);
	void SortMessageIDs();
	bool IsMessageRegistered(std::string name);
	bool IsMessageRegistered(uint16_t id);
	net_message_definition_t* GetRegisteredMessageByName(std::string name);
	net_message_definition_t* GetRegisteredMessageByID(uint8_t id);

	//Update loop
	void ClearConnectionMesssages();
	void ProcessIncoming();
	void ProcessOutgoing();

	void ProcessMessage(NetMessage message, NetAddress from);

protected:
	void SendPacketToConnection(uint8_t connIndex, NetPacket* packet, uint8_t numMessages);
	NetConnection* m_connections[MAX_CONNECTIONS] = {nullptr};
	std::vector<net_message_definition_t*> m_registeredMessages;
	//std::map<std::string, NetSessionMessageCB> m_registeredMessages;
	UDPSocket* m_socket;		// your node in the graph? 


};