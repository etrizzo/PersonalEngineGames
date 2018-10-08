#include "NetPacket.hpp"

NetPacket::NetPacket()
	:BytePacker(LITTLE_ENDIAN)
{
}

void NetPacket::WriteHeader(packet_header_t const & header)
{
	unsigned int writeHead = GetWrittenByteCount();
	SetWriteHead(0);
	WriteBytes(sizeof(packet_header_t), &header, false);
	SetWriteHead(writeHead);
}

bool NetPacket::ReadHeader(packet_header_t & header)
{
	size_t readBytes = ReadBytes(&header, sizeof(header), false);
	return readBytes == sizeof(packet_header_t);
}

bool NetPacket::WriteMessage(NetMessage const & msg)
{
	return WriteBytes(msg.GetWrittenByteCount(), msg.GetBuffer(), true);
}

bool NetPacket::ReadMessage(NetMessage * outMsg)
{
	//read message + header size
	uint16_t msgSize;
	Read(&msgSize, false);

	//write message header and message
	bool read = outMsg->WriteBytes(msgSize, GetReadHeadLocation(), false);
	return read;
}
