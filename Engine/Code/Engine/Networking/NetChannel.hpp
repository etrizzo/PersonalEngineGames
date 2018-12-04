#pragma once
#include "Engine/Core/EngineCommon.hpp"

class NetMessage;


class NetChannel
{  
public:
	~NetChannel();
	uint16_t GetAndIncrementNextSentSequenceID();

	uint16_t m_nextSentSequenceID = 0;												// used for sending
	uint16_t m_nextExpectedSequenceID = 0;											// used for receiving
	std::vector<NetMessage*> m_outOfOrderMessages = std::vector<NetMessage*>();		// used for receiving
};