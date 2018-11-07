#include "PacketTracker.hpp"

PacketTracker::PacketTracker(uint16_t ack)
{
	m_ack = ack;
	m_sentMS = GetCurrentTimeMilliseconds();
}

void PacketTracker::SetAckAndTimestamp(uint16_t ack)
{
	m_ack = ack;
	m_sentMS = GetCurrentTimeMilliseconds();
	m_isValid = true;
}

void PacketTracker::Invalidate()
{
	m_isValid = false;
}
