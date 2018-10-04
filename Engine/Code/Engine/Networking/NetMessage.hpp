#pragma once
#include "Engine/Core/EngineCommon.hpp"

class NetConnection;
struct net_message_definition_t;

#define MESSAGE_MTU (1 * KB)
#define INVALID_MESSAGE_ID ((uint8_t) -1)

struct net_sender_t
{
	net_sender_t(NetConnection* connection);
	NetConnection* m_connection;
};


//this seems like mostly a wrapper for bytepacker so far...
class NetMessage : public BytePacker 
{
public:
	NetMessage();		//should probably only be used when getting data, not sending?
	NetMessage(std::string msg);	//constructs with initial string - signifies what kind of message?

	//template this however you do that
	//void Write(float val);
	//void WriteString(std::string str);

	void WriteHeader();
	void WriteData(std::string data);

	void ReadString(std::string& outString);


	void SetDefinition(net_message_definition_t* def);
	std::string GetNameFromDefinition();
	uint16_t GetIDFromDefinition();

	std::string m_msgName;
	uint16_t m_msgID;
	uint16_t m_msgSize;

	net_message_definition_t* m_definition;

};


typedef void (*NetSessionMessageCB) ( NetMessage msg, net_sender_t const& from);	//callback for different messages the session can doooo


struct net_message_definition_t{
	uint8_t m_messageID		= INVALID_MESSAGE_ID;
	std::string m_messageName	= "";
	NetSessionMessageCB* m_messageCB;
};



bool CompareDefinitionsByName(net_message_definition_t* a, net_message_definition_t* b);

