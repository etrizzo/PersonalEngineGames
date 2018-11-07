#include "Engine/Networking/NetSession.hpp"

class PacketTracker{
public:
	PacketTracker(uint16_t ack);

	void SetAckAndTimestamp(uint16_t ack);
	void Invalidate();

	uint16_t m_ack = INVALID_PACKET_ACK;
	unsigned int m_sentMS;
	bool m_isValid = false;
};