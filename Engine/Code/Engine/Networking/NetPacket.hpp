#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Networking/NetMessage.hpp"
#include "Engine/Networking/UDPSocket.hpp"

//struct packet_header_t{
//	uint8_t m_senderConnectionIndex;
//	uint8_t m_unreliableMessageCount;
//};

struct packet_header_t
{
	// old; 
	uint8_t m_senderConnectionIndex; 

	// new
	uint16_t m_ack; 
	uint16_t m_lastReceivedAck; 
	uint16_t m_previousReceivedAckBitfield; 

	// old
	uint8_t m_unreliableMessageCount; 
}; 

// A packet is a package sent over the network
// It allows you to write/read data to it
// also contains who the recipient and/or sender are
// Contains extra functionality for writing headers/messages.
//
// Think of it as the box goods are sent in; 
// UES LITTLE_ENDIAN
class NetPacket : public BytePacker
{
public:
	NetPacket();
	void WriteHeader( packet_header_t const &header );
	bool ReadHeader( packet_header_t &header ); 

	// writes/reads the following
	// - uint16_t header_and_msg_size (header size is always 1 for now, msg size is msg.get_total_written_bytes())
	// - header
	//   - uint8_t msg_definition_index  
	bool WriteMessage( NetMessage const &msg ); 
	bool ReadMessage( NetMessage *out_msg ); 

public:

	// informtaion so we know who it came from
	// when receiving, and who we're sending to when sending
	// ...
};
