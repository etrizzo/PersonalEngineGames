#pragma once
#include "Engine/Core/EngineCommon.hpp"

class NetConnection;
struct net_message_definition_t;

enum eNetMessageOptionBit : unsigned int 
{
	NETMESSAGE_OPTION_UNRELIABLE_REQUIRES_CONNECTION	= 0U,
	NETMESSAGE_OPTION_CONNECTIONLESS					= BIT_FLAG(0), // task14
	NETMESSAGE_OPTION_RELIABLE							= BIT_FLAG(1), // task15
	NETMESSAGE_OPTION_IN_ORDER							= BIT_FLAG(2), // task16

	 // convenience
	 NETMSSAGE_OPTION_RELIALBE_IN_ORDER	= NETMESSAGE_OPTION_RELIABLE | NETMESSAGE_OPTION_IN_ORDER, 
};
typedef unsigned int eNetMessageOptions; 

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

	bool RequiresConnection() const;
	bool IsReliable() const;

	//template this however you do that
	//void Write(float val);
	//void WriteString(std::string str);
	void IncrementMessageSize(uint16_t size);
	void MoveWriteHeadPastHeader();
	void WriteHeader();
	uint16_t GetHeaderSize();
	void WriteData(std::string data);

	void ReadString(std::string& outString);


	void SetDefinition(net_message_definition_t* def);
	std::string GetNameFromDefinition();
	uint16_t GetIDFromDefinition();

	std::string m_msgName;
	uint16_t m_msgID;
	uint16_t m_msgSize;
	uint16_t m_reliableID = 0;

	int m_lastSentTimeMS = 0;

	net_message_definition_t* m_definition;

};


typedef void (*NetSessionMessageCB) ( NetMessage msg, net_sender_t const& from);	//callback for different messages the session can doooo


struct net_message_definition_t{
	uint8_t m_messageID		= INVALID_MESSAGE_ID;
	std::string m_messageName	= "";
	NetSessionMessageCB m_messageCB;
	eNetMessageOptions m_messageOptions = NETMESSAGE_OPTION_UNRELIABLE_REQUIRES_CONNECTION;
	int m_fixedIndex = -1;

	bool IsConnectionless() const;
	bool IsReliable() const;
	bool IsInOrder() const;
};



bool CompareDefinitionsByName(net_message_definition_t* a, net_message_definition_t* b);

