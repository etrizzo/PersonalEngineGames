#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Networking/UDPSocket.hpp"

#define INVALID_PACKET_ACK (0xffff)

class NetMessage;
class NetSession;

class NetConnection 
{
public:
	NetConnection(NetSession* owningSession, uint8_t indexInSession, NetAddress addr);
	
	void Update();
	void ClearMessageQueue();
	void Send(NetMessage* msg);
	void SetHeartbeat(float hz);
	void SetSendRate(float hz = 0.f);
	NetAddress GetAddress() const;

	bool CanSendToConnection();

	//Accessors
	uint16_t		GetNextSentAck()		const;
	uint16_t		GetLastReceivedAck()	const;
	uint16_t		GetAckBitfield()		const;
	unsigned int	GetLastSendTimeMS()		const;
	unsigned int	GetLastReceivedTimeMS()	const;
	float			GetLossRate()			const;
	float			GetRTT()				const;

	float	GetLastSendTimeSeconds()		const;
	float	GetLastReceivedTimeSeconds()	const;

	std::vector<NetMessage*> m_messagesToSend;
	NetAddress m_address;
	uint8_t m_indexInSession;
	NetSession *m_owningSession; 
	StopWatch m_heartbeatTimer;
	StopWatch m_sendRateTimer;
	float m_connectionSendRateHZ = 0;

	bool m_isLocal = false;

protected:
	// sending - updated during a send/flush
	uint16_t m_nextSentACK                  = 0U; 

	// receiving - updated during a process_packet
	uint16_t m_lastReceivedACK               = INVALID_PACKET_ACK; 
	uint16_t m_previousReceivedACKBitfield   = 0U; 

	// Analytics
	unsigned int m_lastSendTimeMS		= 0;
	unsigned int m_lastReceivedTimeMS	= 0;

	// note these variables are unrelated to the debug sim on the session
	// but will end up reflecting those numbers.
	float m_lossRate	= 0.0f;       // loss rate we perceive to this connection
	float m_rtt			= 0.0f;       // latency perceived on this connection


};
