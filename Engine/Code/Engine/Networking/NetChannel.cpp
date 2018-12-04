#include "NetChannel.hpp"

NetChannel::~NetChannel()
{
	for (NetMessage* msg : m_outOfOrderMessages){
		delete (msg);
	}
}

uint16_t NetChannel::GetAndIncrementNextSentSequenceID()
{
	uint16_t next = m_nextSentSequenceID;
	m_nextSentSequenceID++;
	return next;
}
