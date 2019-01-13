#include "Engine/Networking/NetSession.hpp"

#define MAX_RELIABLES_PER_PACKET (32)


class PacketTracker{
public:
	PacketTracker(uint16_t ack, unsigned int sendTimeMS);

	void SetAckAndTimestamp(uint16_t ack, unsigned int sendTimeMS);
	void Invalidate();
	void AddTrackedReliable(NetMessage* reliableMsg);
	void Reset();

	uint16_t m_ack = INVALID_PACKET_ACK;
	unsigned int m_sentMS;
	bool m_isValid = false;

	uint16_t m_numReliablesInPacket = 0;
	uint16_t m_sentReliableIDs[MAX_RELIABLES_PER_PACKET];
};