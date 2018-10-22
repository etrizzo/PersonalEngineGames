#include "NetConnection.hpp"
#include "Engine/Networking/NetPacket.hpp"
#include "Engine/Networking/NetSession.hpp"



NetConnection::NetConnection(NetSession * owningSession, uint8_t indexInSession, NetAddress addr)
{
	m_owningSession = owningSession;
	m_indexInSession = indexInSession;
	m_address = addr;
	m_heartbeatTimer = StopWatch(GetMasterClock());
	m_sendRateTimer = StopWatch(GetMasterClock());
	SetSendRate();
}

void NetConnection::Update()
{
	if (m_heartbeatTimer.DecrementAll()){
		//ConsolePrintf(RGBA::RED.GetColorWithAlpha(165), "Sending heartbeat.");
		NetMessage* msg = new NetMessage("heartbeat"); 
		Send(msg);
	}
}

void NetConnection::ClearMessageQueue()
{
	m_messagesToSend.clear();
}

void NetConnection::Send(NetMessage * msg)
{
	msg->SetDefinition(m_owningSession->GetRegisteredMessageByName(msg->m_msgName));
	msg->WriteHeader();
	m_messagesToSend.push_back(msg);
}

void NetConnection::SetHeartbeat(float hz)
{
	if (hz == 0.f){
		hz = DEFAULT_HEARTBEAT;
	}
	float seconds = 1.f/ hz;
	m_heartbeatTimer.SetTimer(seconds);
}

void NetConnection::SetSendRate(float hz)
{
	m_connectionSendRateHZ = hz;
	float hzToSetFrom = hz;
	if (m_connectionSendRateHZ == 0.f){
		hzToSetFrom = m_owningSession->GetSessionSendRateHZ();
	} else {
		hzToSetFrom = Min(m_connectionSendRateHZ, m_owningSession->GetSessionSendRateHZ());
	}
	if (hzToSetFrom == 0.f){
		hzToSetFrom = DEFAULT_SESSION_SEND_RATE_HZ;
	}
	float setFromSeconds = 1.f/ hzToSetFrom;
	m_sendRateTimer.SetTimer(setFromSeconds);
}

NetAddress NetConnection::GetAddress() const
{
	return m_address;
}

bool NetConnection::CanSendToConnection()
{
	return m_sendRateTimer.CheckAndReset();
}

uint16_t NetConnection::GetNextSentAck() const
{
	return m_nextSentACK;
}

uint16_t NetConnection::GetLastReceivedAck() const
{
	return m_lastReceivedACK;
}

uint16_t NetConnection::GetAckBitfield() const
{
	return m_previousReceivedACKBitfield;
}

unsigned int NetConnection::GetLastSendTimeMS() const
{
	return m_lastSendTimeMS;
}

unsigned int NetConnection::GetLastReceivedTimeMS() const
{
	return m_lastReceivedTimeMS;
}

float NetConnection::GetLossRate() const
{
	return m_lossRate;
}

float NetConnection::GetRTT() const
{
	return m_rtt;
}

float NetConnection::GetLastSendTimeSeconds() const
{
	return (float) GetLastSendTimeMS() * .001f;
}

float NetConnection::GetLastReceivedTimeSeconds() const
{
	return  (float) GetLastReceivedTimeMS() * .001f;
}
