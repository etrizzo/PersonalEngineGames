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
}

void NetMessage::WriteHeader()
{
	uint8_t id = m_definition->m_messageID;
	unsigned int writeHead = GetWrittenByteCount();
	SetWriteHead(0);
	Write((uint16_t) (sizeof(uint8_t) + m_msgSize), false);
	Write(id, false);
	SetWriteHead(writeHead);
}

void NetMessage::WriteData(std::string data)
{
	if (m_writeHead == 0){
		AdvanceWriteHead(sizeof(uint8_t) + sizeof(uint16_t));		//advance bytes for message ID & message size
	}

	size_t bytesAdded = WriteString(data.c_str());
	m_msgSize+= (uint16_t) bytesAdded;
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
	return a->m_messageName < b->m_messageName;
}
