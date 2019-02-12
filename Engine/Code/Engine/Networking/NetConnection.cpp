#include "NetConnection.hpp"
#include "Engine/Networking/NetPacket.hpp"
#include "Engine/Networking/NetSession.hpp"
#include "Engine/Networking/PacketTracker.hpp"
#include "Engine/Networking/NetChannel.hpp"





NetConnection::NetConnection(NetSession * owningSession, net_connection_info_t info)
{
	m_info = info;
	m_owningSession = owningSession;

	m_heartbeatTimer = StopWatch(GetMasterClock());
	m_sendRateTimer = StopWatch(GetMasterClock());
	m_lastReceivedTimeMS = (unsigned int) (GetMasterClock()->GetCurrentSeconds() * 1000.f);
	SetSendRate();
	for (int i = 0; i < NUM_ACKS_TRACKED; i++){
		m_trackedSentPackets[i] = new PacketTracker(INVALID_PACKET_ACK, 0U);
	}
	for (int i = 0; i < MAX_MESSAGE_CHANNELS; i++){
		m_channels[i] = new NetChannel();
	}
}

NetConnection::NetConnection(NetSession * owningSession, uint8_t indexInSession, NetAddress addr)
{
	m_owningSession = owningSession;
	m_info = net_connection_info_t();
	m_info.m_sessionIndex = indexInSession;
	m_info.m_address = addr;
	m_heartbeatTimer = StopWatch(GetMasterClock());
	m_sendRateTimer = StopWatch(GetMasterClock());
	m_lastReceivedTimeMS = (unsigned int) (GetMasterClock()->GetCurrentSeconds() * 1000.f);
	SetSendRate();
	for (int i = 0; i < NUM_ACKS_TRACKED; i++){
		m_trackedSentPackets[i] = new PacketTracker(INVALID_PACKET_ACK, 0U);
	}
	for (int i = 0; i < MAX_MESSAGE_CHANNELS; i++){
		m_channels[i] = new NetChannel();
	}
}

NetConnection::~NetConnection()
{
	for(NetChannel* channel : m_channels)
	{
		delete channel;

	}
}

void NetConnection::Update()
{
	//if haven't received a packet in a while, disconnect
	if (!IsMe()){
		float elapsedSeconds = (GetMasterClock()->GetCurrentSeconds() - ((float) m_lastReceivedTimeMS * .001f));
		if ( elapsedSeconds > DEFAULT_CONNECTION_TIMEOUT){
			ConsolePrintf(RGBA::RED, "Disconnecting connection %i after %f seconds", m_info.m_sessionIndex, elapsedSeconds);
			Disconnect();
		}
	}

	//if state has changed this frame, update it and send to everybody
	if (m_previousState != m_state){
		//set state & send update state msg
		m_previousState = m_state;
		for(NetConnection* conn : m_owningSession->m_boundConnections){
			NetMessage* update = new NetMessage("update_connection_state", m_owningSession);
			//update->SetDefinitionFromSession(m_owningSession);
			update->Write(m_info.m_sessionIndex);
			update->Write((uint8_t) m_state);
			update->IncrementMessageSize(sizeof(uint8_t) + sizeof(uint8_t));
			conn->Send(update);
		}
	}


	unsigned int deltasecondsMS = (unsigned int) (GetMasterClock()->GetDeltaSeconds() * 1000.f);
	for (NetMessage* unconfirmed : m_unconfirmedReliableMessages){
		unconfirmed->IncrementAge(deltasecondsMS);
	}
	if (m_heartbeatTimer.DecrementAll()){
		//ConsolePrintf(RGBA::RED.GetColorWithAlpha(165), "Sending heartbeat.");
		for (NetConnection* conn : m_owningSession->m_boundConnections){
			NetMessage* msg = new NetMessage("heartbeat"); 
			if (IsHost()){
				//Send your current time
				msg->Write(m_owningSession->m_currentClientTimeMS);
				msg->IncrementMessageSize(sizeof(unsigned int));
			} else {
				msg->Write(0U);
				msg->IncrementMessageSize(sizeof(unsigned int));
			}
			conn->Send(msg);
		}
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


		if (msg->IsInOrder()){
			NetChannel* channel = GetChannelByIndex(msg->m_definition->m_channelID);
			msg->m_sequenceID = channel->GetAndIncrementNextSentSequenceID();
		}

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
	return m_info.m_address;
}

uint8_t NetConnection::GetConnectionIndex() const
{
	return m_info.m_sessionIndex;
}

std::string NetConnection::GetConnectionIDAsString() const
{
	return std::string(m_info.m_id);
}

bool NetConnection::IsMe() const
{
	return (m_owningSession->m_myConnection == this);
}

bool NetConnection::IsHost() const
{
	return (m_owningSession->m_hostConnection == this);
}

bool NetConnection::IsClient() const
{
	return !IsHost();
}

bool NetConnection::IsValidConnection() const
{
	return m_info.m_sessionIndex != INVALID_CONNECTION_INDEX;
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
	tracker->SetAckAndTimestamp(header.m_ack, m_owningSession->GetNetTimeMS());
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
				unsigned int currentMS = m_owningSession->GetNetTimeMS();
				UpdateRTT(currentMS - tracker->m_sentMS );
				m_recievedPackets++;
				if (m_unconfirmedReliableMessages.size() > 0){
					
					//update received reliables
					for (unsigned int i = 0; i < tracker->m_numReliablesInPacket; i++){
						uint16_t reliableID = tracker->m_sentReliableIDs[i];
						for (int unconfirmedIdx = (int) m_unconfirmedReliableMessages.size() - 1; unconfirmedIdx >= 0; unconfirmedIdx--){
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
	if (m_rttMS == 0.f){
		m_rttMS = RTTforConfirmedPacketMS;
	} else {
		m_rttMS = (unsigned int) Interpolate((int) RTTforConfirmedPacketMS, (int)m_rttMS, .9f);
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

unsigned int NetConnection::GetRTT() const
{
	return m_rttMS;
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
		id = (uint16_t) INVALID_RELIABLE_ID;
	} else {
		id = oldest->m_reliableID;
	}
	return id;
}

bool NetConnection::CanSendNewReliable() const
{
	uint16_t nextID = m_nextSentReliableID;
	uint16_t oldestUnconfirmedID = GetOldestUnconfirmedReliable(); // probably just element [0]; 
	if (oldestUnconfirmedID == (uint16_t) INVALID_RELIABLE_ID){
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
		m_highestReceivedReliableID = (uint16_t) Max(newReliableID, m_highestReceivedReliableID);
	}
	//hat problem - counting on the other person to only send you a reliable id if they've received everything before your reliable window
	uint16_t minimum_id = newReliableID - RELIABLE_WINDOW + 1;

	int backIndex = (int) m_receivedReliableIDs.size() - 1;
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

void NetConnection::AddReceivedInOrderMessage(NetMessage * msg)
{
	NetChannel* channel = GetChannelByIndex(msg->m_definition->m_channelID);
	if (channel != nullptr){
		////check if the ID just processed was already in the out of order array
		//for (int i = channel->m_outOfOrderMessages.size() - 1; i >= 0; i--){
		//	if (channel->m_outOfOrderMessages[i]->m_sequenceID <= channel->m_nextExpectedSequenceID){
		//		//messageToProcess = channel->m_outOfOrderMessages[i];
		//		ConsolePrintf("just processed %i, removing dup from array", channel->m_outOfOrderMessages[i]->m_sequenceID);
		//		RemoveAtFast(channel->m_outOfOrderMessages, i);
		//		//break;
		//	}
		//}
		channel->m_nextExpectedSequenceID++;
		//ConsolePrintf(RGBA::GREEN, "Expected id is %i",channel->m_nextExpectedSequenceID);
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

NetChannel * NetConnection::GetChannelByIndex(uint8_t channelIndex)
{
	if (channelIndex < MAX_MESSAGE_CHANNELS){
		return m_channels[channelIndex];
	}
	return nullptr;
}

uint16_t NetConnection::GetNextExpectedIDForChannel(uint8_t channelIndex)
{
	NetChannel* channel = GetChannelByIndex(channelIndex);
	if (channel != nullptr){
		return channel->m_nextExpectedSequenceID;
	} else {
		return 0;
	}
}

bool NetConnection::IsMessageNextExpectedInSequence(NetMessage * msg)
{
	NetChannel* channel = GetChannelByIndex(msg->m_definition->m_channelID);
	if (channel != nullptr){
		return msg->m_sequenceID == channel->m_nextExpectedSequenceID;
	} else {
		return false;
	}
}

void NetConnection::ProcessOutOfOrderMessagesForChannel(uint8_t channelIndex)
{
	NetChannel* channel = GetChannelByIndex(channelIndex);
	NetMessage* messageToProcess = nullptr;

	//ConsolePrintf("Searching %i number of out of order messages for seqID %i on channel %i", channel->m_outOfOrderMessages.size(), channel->m_nextExpectedSequenceID, channelIndex);
	do{
		messageToProcess = nullptr;
		//if the out of order messages list contains the next expected message, pull it out
		for (int i = (int) channel->m_outOfOrderMessages.size() - 1; i >= 0; i--){
			if (channel->m_outOfOrderMessages[i]->m_sequenceID == channel->m_nextExpectedSequenceID){
				messageToProcess = channel->m_outOfOrderMessages[i];
				//ConsolePrintf("Removing message w/ sequence id %i from vector", channel->m_outOfOrderMessages[i]->m_sequenceID);
				RemoveAtFast(channel->m_outOfOrderMessages, i);
				break;
			} 
			else if (channel->m_outOfOrderMessages[i]->m_sequenceID < channel->m_nextExpectedSequenceID){
				//ConsolePrintf("Removing already processed message %i from vector", channel->m_outOfOrderMessages[i]->m_sequenceID);
				RemoveAtFast(channel->m_outOfOrderMessages, i);
			}
		}

		//and process it, update next expected index
		if (messageToProcess != nullptr){
			//if (!HasReceivedReliable(messageToProcess->m_reliableID)){
				if (messageToProcess->m_sequenceID == channel->m_nextExpectedSequenceID){
					channel->m_nextExpectedSequenceID++;
				}
				net_message_definition_t* definition = messageToProcess->m_definition;
				((definition->m_messageCB))( *messageToProcess, net_sender_t(this));
				AddReceivedReliable(messageToProcess->m_reliableID);
				//ConsolePrintf("Processing reliable %i from vector. Now expecting %i", messageToProcess->m_sequenceID, channel->m_nextExpectedSequenceID);
			//}
		}

	} while (messageToProcess != nullptr);
}

void NetConnection::AddOutOfOrderMessage(NetMessage * msg)
{
	NetChannel* channel = GetChannelByIndex(msg->m_definition->m_channelID);
	//for (int i = channel->m_outOfOrderMessages.size() - 1; i >= 0; i--){
	//	if (channel->m_outOfOrderMessages[i]->m_sequenceID == msg->m_sequenceID){
	//		//we've already got a copy of the out of order message
	//		ConsolePrintf("attempting to add same message out of order vector %i", msg->m_sequenceID );
	//		return;
	//	}
	//}
	if (channel != nullptr){
		//NetMessage* copy = new NetMessage(msg);
		//copy->WriteBytes(msg->GetWrittenByteCount(), msg->GetBuffer());
		//uint8_t messageType;
		//copy->Read(&messageType, false);
		//copy->m_definition = msg->m_definition;
		//uint16_t reliableID;
		//copy->Read(&reliableID);
		//copy->m_reliableID = reliableID;
		//uint16_t seqID;
		//copy->Read(&seqID);
		//copy->m_sequenceID = seqID;


		channel->m_outOfOrderMessages.push_back(msg);
		//ConsolePrintf("Adding %i to out of order vector on channel %i", msg->m_sequenceID, msg->m_definition->m_channelID);
	} else {
		ConsolePrintf(RGBA::RED, "No channel (%i) for message.", msg->m_definition->m_channelID);
	}
}
