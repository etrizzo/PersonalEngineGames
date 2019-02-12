#include "PacketTracker.hpp"

PacketTracker::PacketTracker(uint16_t ack, unsigned int sendTimeMS)
{
	m_ack = ack;
	m_sentMS = sendTimeMS;
	for (int i = 0; i < MAX_RELIABLES_PER_PACKET; i++){
		m_sentReliableIDs[i] = (uint16_t) INVALID_RELIABLE_ID;
	}
}

void PacketTracker::SetAckAndTimestamp(uint16_t ack, unsigned int sendTimeMS)
{
	m_ack = ack;
	m_sentMS = sendTimeMS;
	m_isValid = true;
}

void PacketTracker::Invalidate()
{
	m_isValid = false;
	Reset();
}

void PacketTracker::AddTrackedReliable(NetMessage * reliableMsg)
{
	if (m_numReliablesInPacket < MAX_RELIABLES_PER_PACKET){
		m_sentReliableIDs[m_numReliablesInPacket] = reliableMsg->m_reliableID;
		m_numReliablesInPacket++;
	} else {
		ConsolePrintf(RGBA::RED, "Too many reliables in packet.");
	}
}

void PacketTracker::Reset()
{
	//m_isValid = false;
	m_numReliablesInPacket = 0;
	for (int i = 0; i < MAX_RELIABLES_PER_PACKET; i++){
		m_sentReliableIDs[i] = (uint16_t) INVALID_RELIABLE_ID;
	}

}
