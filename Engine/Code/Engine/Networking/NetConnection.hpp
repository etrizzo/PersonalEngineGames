#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Networking/UDPSocket.hpp"
#include "Engine/Networking/NetPacket.hpp"


class NetMessage;
class NetSession;
class PacketTracker;

#define NUM_ACKS_TRACKED (16)


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

	bool IsValidConnection() const;

	bool CanSendToConnection();

	void IncrementSendAck();
	void MarkSendTime();
	PacketTracker* AddTrackedPacketOnSend(const packet_header_t& header);

	void MarkReceiveTime();
	void UpdateReceivedAcks(uint16_t newReceivedAck);
	//returns true if the packet was tracked and unconfirmed
	bool ConfirmPacketReceived(uint16_t newReceivedAck);

	void UpdateRTT(unsigned int RTTforConfirmedPacketMS);

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

	bool		ShouldSendReliableMessage(NetMessage* msg)		const;
	uint16_t	GetAndIncrementNextReliableID();
	uint16_t	GetOldestUnconfirmedReliable() const;
	bool		CanSendNewReliable() const;
	void		MarkMessageAsSentForFirstTime(NetMessage* msg);

	void AddReceivedReliable(uint16_t newReliableID);

	bool HasReceivedReliable(uint16_t reliableID);

	std::vector<NetMessage*> m_unsentUnreliableMessages = std::vector<NetMessage*>();
	NetAddress m_address;
	uint8_t m_indexInSession;
	NetSession *m_owningSession; 
	StopWatch m_heartbeatTimer;
	StopWatch m_sendRateTimer;
	float m_connectionSendRateHZ = 0;
	unsigned int m_lostPackets = 0;
	unsigned int m_recievedPackets = 0;

	std::vector<NetMessage*> m_unconfirmedReliableMessages = std::vector<NetMessage*>();
	std::vector<NetMessage*> m_unsentReliableMessages = std::vector<NetMessage*>();
	std::vector<NetMessage*> m_sentReliableMessages = std::vector<NetMessage*>();
	std::vector<uint16_t> m_receivedReliableIDs = std::vector<uint16_t>();

	bool m_isLocal = false;

protected:
	PacketTracker* m_trackedSentPackets[NUM_ACKS_TRACKED];
	// sending - updated during a send/flush
	uint16_t m_nextSentACK                  = 0U; 

	// receiving - updated during a process_packet
	uint16_t m_highestReceivedACK               = INVALID_PACKET_ACK; 
	uint16_t m_previousReceivedACKBitfield   = 0U; 

	// Analytics
	unsigned int m_lastSendTimeMS		= 0;
	unsigned int m_lastReceivedTimeMS	= 0;

	// note these variables are unrelated to the debug sim on the session
	// but will end up reflecting those numbers.
	float m_lossRate	= 0.0f;       // loss rate we perceive to this connection
	float m_rtt			= 0.0f;       // latency perceived on this connection

	uint16_t m_nextSentReliableID = 0;
	uint16_t m_highestReceivedReliableID = 0U;

};
