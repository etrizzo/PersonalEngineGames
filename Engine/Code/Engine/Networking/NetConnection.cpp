#include "NetConnection.hpp"
#include "Engine/Networking/NetPacket.hpp"
#include "Engine/Networking/NetSession.hpp"
#include "Engine/Networking/PacketTracker.hpp"



NetConnection::NetConnection(NetSession * owningSession, uint8_t indexInSession, NetAddress addr)
{
	m_owningSession = owningSession;
	m_indexInSession = indexInSession;
	m_address = addr;
	m_heartbeatTimer = StopWatch(GetMasterClock());
	m_sendRateTimer = StopWatch(GetMasterClock());
	SetSendRate();
	for (int i = 0; i < NUM_ACKS_TRACKED; i++){
		m_trackedSentPackets[i] = new PacketTracker(INVALID_PACKET_ACK);
	}
}

void NetConnection::Update()
{
	unsigned int deltasecondsMS = (unsigned int) (GetMasterClock()->GetDeltaSeconds() * 1000.f);
	for (NetMessage* unconfirmed : m_unconfirmedReliableMessages){
		unconfirmed->IncrementAge(deltasecondsMS);
	}
	if (m_heartbeatTimer.DecrementAll()){
		//ConsolePrintf(RGBA::RED.GetColorWithAlpha(165), "Sending heartbeat.");
		NetMessage* msg = new NetMessage("heartbeat"); 
		Send(msg);
	}
	if (m_recievedPackets > 0){
		m_lossRate = (float) m_lostPackets / (float) m_recievedPackets;
	}
}

void NetConnection::ClearMessageQueue()
{
	m_unsentUnreliableMessages.clear();
	m_unsentReliableMessages.clear();
}

void NetConnection::Send(NetMessage * msg)
{
	msg->SetDefinition(m_owningSession->GetRegisteredMessageByName(msg->m_msgName));
	msg->WriteHeader();
	if (msg->IsReliable()) {
		//don't actually assign reliable id until it gets sent for Reasons
		m_unsentReliableMessages.push_back(msg);
	} else {
		m_unsentUnreliableMessages.push_back(msg);
	}
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

bool NetConnection::IsValidConnection() const
{
	return m_indexInSession != INVALID_CONNECTION_INDEX;
}

bool NetConnection::CanSendToConnection()
{
	return m_sendRateTimer.CheckAndReset();
}

void NetConnection::IncrementSendAck()
{
	m_nextSentACK++;
}

void NetConnection::MarkSendTime()
{
	m_lastSendTimeMS = (unsigned int) (GetCurrentTimeSeconds() * 1000.f);
}

PacketTracker * NetConnection::AddTrackedPacketOnSend(const packet_header_t & header)
{
	uint16_t idx = (header.m_ack) % NUM_ACKS_TRACKED;
	PacketTracker* tracker = m_trackedSentPackets[idx];
	if (tracker->m_isValid){
		//give up on this packet :(
		m_lostPackets++;
		tracker->Reset();
	}
	tracker->SetAckAndTimestamp(header.m_ack);
	return tracker;
}

void NetConnection::MarkReceiveTime()
{
	m_lastReceivedTimeMS = (unsigned int) (GetCurrentTimeSeconds() * 1000.f);
}

void NetConnection::UpdateReceivedAcks(uint16_t newReceivedAck)
{
	//if (newReceivedAck > m_lastReceivedACK || m_lastReceivedACK == INVALID_PACKET_ACK){
	//	m_lastReceivedACK = newReceivedAck;
	//} else {
	//	//update bitfield
	//}

	uint16_t distance = newReceivedAck - m_highestReceivedACK;
	if (distance == 0){
		//this is an error. ACK increments in order. Should never happen.
		return;
	}
	if (distance < (0xffff / 2)){		//half a cycle away
		m_highestReceivedACK = newReceivedAck;
		// how do I update the bitfield?
		// want to left shift by the distance
		m_previousReceivedACKBitfield <<= distance;  //giving self more space in the bitfield.
		m_previousReceivedACKBitfield |= ( 1 << (distance - 1)); 	//set the old highest bit.
	} else {
		//got an older ACK than highest received ack. Which bit do we set in history?
		distance = m_highestReceivedACK - newReceivedAck; 	//distance from highest
		m_previousReceivedACKBitfield |= (1 << (distance - 1)); 	//set bit in history
		//may want to check that bit WAS zero, otherwise you double processed a packet, which is bad.
	}
}

bool NetConnection::ConfirmPacketReceived(uint16_t newReceivedAck)
{
	for (PacketTracker* tracker : m_trackedSentPackets){
		if (tracker->m_ack == newReceivedAck){
			if (tracker->m_isValid){
				unsigned int currentMS = GetCurrentTimeMilliseconds();
				UpdateRTT(currentMS - tracker->m_sentMS );
				m_recievedPackets++;
				if (m_unconfirmedReliableMessages.size() > 0){
					
					//update received reliables
					for (unsigned int i = 0; i < tracker->m_numReliablesInPacket; i++){
						uint16_t reliableID = tracker->m_sentReliableIDs[i];
						for (int unconfirmedIdx = m_unconfirmedReliableMessages.size() - 1; unconfirmedIdx >= 0; unconfirmedIdx--){
							NetMessage* unconfirmed = m_unconfirmedReliableMessages[unconfirmedIdx];
							if (unconfirmed->m_reliableID == reliableID){
								RemoveAtFast(m_unconfirmedReliableMessages, unconfirmedIdx);
							}
						}
						
					}
				}
				tracker->Invalidate();
				return true;
			} else {
				return false;
			}
		}
	}
	return false;
}

void NetConnection::UpdateRTT(unsigned int RTTforConfirmedPacketMS)
{
	if (m_rtt == 0.f){
		m_rtt = RTTforConfirmedPacketMS;
	} else {
		m_rtt = Interpolate(RTTforConfirmedPacketMS, m_rtt, .9f);
	}
}

uint16_t NetConnection::GetNextSentAck() const
{
	return m_nextSentACK;
}

uint16_t NetConnection::GetLastReceivedAck() const
{
	return m_highestReceivedACK;
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

bool NetConnection::ShouldSendReliableMessage(NetMessage * msg) const
{
	for (NetMessage* unconfirmed : m_unconfirmedReliableMessages){
		if (unconfirmed == msg ){
			if (msg->IsOldEnoughToResend()){
				return true;
			}
		}
	} 
	return false;
}

uint16_t NetConnection::GetAndIncrementNextReliableID()
{
	uint16_t next = m_nextSentReliableID;
	m_nextSentReliableID++;
	return next;
}

uint16_t NetConnection::GetOldestUnconfirmedReliable() const
{
	NetMessage* oldest = nullptr;
	if (m_unconfirmedReliableMessages.size() > 0){
		oldest = m_unconfirmedReliableMessages[0];
	}
	for (NetMessage* msg : m_unconfirmedReliableMessages){
		if (oldest->m_timeSinceLastSentMS <  msg->m_timeSinceLastSentMS){
			oldest = msg;
		}
	}
	uint16_t id; 
	if (oldest == nullptr){
		id = INVALID_RELIABLE_ID;
	} else {
		id = oldest->m_reliableID;
	}
	return id;
}

bool NetConnection::CanSendNewReliable() const
{
	uint16_t nextID = m_nextSentReliableID;
	uint16_t oldestUnconfirmedID = GetOldestUnconfirmedReliable(); // probably just element [0]; 
	if (oldestUnconfirmedID = INVALID_RELIABLE_ID){
		oldestUnconfirmedID = nextID;
	}

	uint16_t diff = nextID - oldestUnconfirmedID; 
	return (diff < RELIABLE_WINDOW); 
}

void NetConnection::MarkMessageAsSentForFirstTime(NetMessage * msg)
{
	for (int i = 0; i < m_unsentReliableMessages.size(); i++){
		if (m_unsentReliableMessages[i] == msg){
			m_unsentReliableMessages[i] = nullptr;	//remove from unsent list
		}
	}
	m_unconfirmedReliableMessages.push_back(msg);
	msg->ResetAge();
}

void NetConnection::AddReceivedReliable(uint16_t newReliableID)
{
	m_receivedReliableIDs.push_back(newReliableID);

	//if this is the first reliable you've received, set it here
	if (m_highestReceivedReliableID == INVALID_RELIABLE_ID){
		m_highestReceivedReliableID = newReliableID;
	} else {
		m_highestReceivedReliableID = Max(newReliableID, m_highestReceivedReliableID);
	}
	//hat problem - counting on the other person to only send you a reliable id if they've received everything before your reliable window
	uint16_t minimum_id = newReliableID - RELIABLE_WINDOW + 1;

	int backIndex = m_receivedReliableIDs.size() - 1;
	int numRemoved = 0;
	for(int i = backIndex; i >=0; i--){
		uint16_t oldID = m_receivedReliableIDs[i];
		if (CycleLess(oldID, minimum_id)) {
			//remove the reliable id from the received list
			m_receivedReliableIDs[i] = m_receivedReliableIDs[backIndex];
			backIndex--;
			numRemoved++;
		}
	}

	//purge the removed reliable ids
	for (int i = 0; i < numRemoved; i++){
		m_receivedReliableIDs.pop_back();
	}
}

bool NetConnection::HasReceivedReliable(uint16_t reliableID)
{
	uint16_t maxWindowVal = m_highestReceivedReliableID;
	uint16_t minWindowVal = maxWindowVal - RELIABLE_WINDOW;

	if (CycleLess(reliableID, minWindowVal))
	{
		//if it's less than your window, it's so old you DEFINITELY received it.
		return true;
	} else {
		// if it's within the reliable window, we just have to check if 
		// our list of received reliables already contains it
		for (unsigned int i = 0; i < m_receivedReliableIDs.size(); i++){
			if (m_receivedReliableIDs[i] == reliableID){
				return true;
			}
		}
		//we haven't received it yet!
		return false;
	}
}
