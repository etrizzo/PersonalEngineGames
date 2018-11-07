#include "NetMessage.hpp"

net_sender_t::net_sender_t(NetConnection * connection)
{
	m_connection = connection;
}

NetMessage::NetMessage()
	:BytePacker(LITTLE_ENDIAN)
{
	m_definition = new net_message_definition_t();
}

NetMessage::NetMessage(std::string msg)
	:BytePacker(LITTLE_ENDIAN)
{
	m_definition = new net_message_definition_t();
	m_msgName = msg;
	MoveWriteHeadPastHeader();
}

bool NetMessage::RequiresConnection() const
{
	return !m_definition->IsConnectionless();
}

bool NetMessage::IsReliable() const
{
	return m_definition->IsReliable();
}

void NetMessage::IncrementMessageSize(uint16_t size)
{
	m_msgSize+=size;
}

void NetMessage::MoveWriteHeadPastHeader()
{
							//header size bytes		   [    "msg header"					]
	//msg header looks like:  <2B msg and header size> <1B msgID> <optional 2B reliableID>

							//bytes for size	//header bytes
	uint16_t sizeOfHeader = sizeof(uint16_t) + GetHeaderSize();
	if (m_writeHead == 0){
		AdvanceWriteHead((size_t) sizeOfHeader);		//advance bytes for message ID & message size
	}
}

void NetMessage::WriteHeader()
{
	uint8_t id = m_definition->m_messageID;
	unsigned int writeHead = GetWrittenByteCount();
	SetWriteHead(0);
	Write((uint16_t) (GetHeaderSize() + m_msgSize), false);
	Write(id, false);
	if (m_definition->IsReliable()){
		Write(m_reliableID, false);
	}
	SetWriteHead(writeHead);
}

uint16_t NetMessage::GetHeaderSize()
{
	uint16_t msgSize = sizeof(m_definition->m_messageID);
	if (m_definition->IsReliable()){
		msgSize+= sizeof(m_reliableID);
	}
	return msgSize;
}

void NetMessage::WriteData(std::string data)
{
	size_t bytesAdded = WriteString(data.c_str());
	m_msgSize+= (uint16_t) (bytesAdded);
}

void NetMessage::ReadString(std::string & outString)
{
	char cStr[MESSAGE_MTU];
	BytePacker::ReadString(cStr, MESSAGE_MTU);
	outString = cStr;
}

void NetMessage::SetDefinition(net_message_definition_t* def)
{
	m_definition = def;
}

std::string NetMessage::GetNameFromDefinition()
{
	return m_definition->m_messageName;
}

uint16_t NetMessage::GetIDFromDefinition()
{
	return m_definition->m_messageID;
}

bool CompareDefinitionsByName(net_message_definition_t* a, net_message_definition_t* b)
{
	if (a->m_fixedIndex != -1){
		return false;		//don't fuckin move it bicc
	}
	return a->m_messageName < b->m_messageName;
}

bool net_message_definition_t::IsConnectionless() const
{
	return AreBitsSet(m_messageOptions, NETMESSAGE_OPTION_CONNECTIONLESS);
}

bool net_message_definition_t::IsReliable() const
{
	return AreBitsSet(m_messageOptions, NETMESSAGE_OPTION_RELIABLE);
}

bool net_message_definition_t::IsInOrder() const
{
	return AreBitsSet(m_messageOptions, NETMESSAGE_OPTION_IN_ORDER);
}
