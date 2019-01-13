#include "NetSession.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Networking/NetConnection.hpp"
#include "Engine/Networking/UDPSocket.hpp"
#include "Engine/Networking/NetPacket.hpp"
#include "Engine/Math/Renderer.hpp"
#include "Engine/Networking/PacketTracker.hpp"

NetSession::NetSession()
{
	m_socket = new UDPSocket();
	Startup();
}

void NetSession::Startup()
{
	RegisterMessage( NETMSG_PING, "ping", (NetSessionMessageCB) OnPing , NETMESSAGE_OPTION_CONNECTIONLESS ); 
	RegisterMessage( NETMSG_PONG, "pong", (NetSessionMessageCB) OnPong , NETMESSAGE_OPTION_CONNECTIONLESS ); 
	RegisterMessage( NETMSG_HEARTBEAT, "heartbeat", (NetSessionMessageCB) OnHeartbeat, NETMESSAGE_OPTION_UNRELIABLE_REQUIRES_CONNECTION );
	
	RegisterMessage( NETMSG_JOIN_REQUEST, "join_request", (NetSessionMessageCB) OnJoinRequest, NETMESSAGE_OPTION_CONNECTIONLESS);
	RegisterMessage( NETMSG_JOIN_DENY, "join_deny", (NetSessionMessageCB) OnJoinDeny, NETMESSAGE_OPTION_CONNECTIONLESS);
	RegisterMessage( NETMSG_JOIN_ACCEPT, "join_accept", (NetSessionMessageCB) OnJoinAccept, NETMSSAGE_OPTION_RELIABLE_IN_ORDER);
	RegisterMessage( NETMSG_NEW_CONNECTION, "new_connection", (NetSessionMessageCB) OnNewConnection, NETMSSAGE_OPTION_RELIABLE_IN_ORDER);
	RegisterMessage( NETMSG_JOIN_FINISHED, "join_finished", (NetSessionMessageCB)OnJoinFinished, NETMSSAGE_OPTION_RELIABLE_IN_ORDER);
	RegisterMessage( NETMSG_UPDATE_CONN_STATE, "update_connection_state", (NetSessionMessageCB)OnUpdateConnectionState, NETMSSAGE_OPTION_RELIABLE_IN_ORDER);

	RegisterMessage( NETMSG_HANGUP, "hangup", (NetSessionMessageCB)OnHangup, NETMESSAGE_OPTION_UNRELIABLE_REQUIRES_CONNECTION);
	
	RegisterMessage( NETMSG_UNRELIABLE_TEST, "unreliable_test", (NetSessionMessageCB) OnNetMsgUnreliableTest, NETMESSAGE_OPTION_UNRELIABLE_REQUIRES_CONNECTION);
	RegisterMessage( NETMSG_RELIABLE_TEST, "reliable_test", (NetSessionMessageCB) OnNetMsgReliableTest, NETMESSAGE_OPTION_RELIABLE );
	RegisterMessage( NETMSG_SEQUENCE_TEST, "sequence_test", (NetSessionMessageCB) OnNetMsgSequenceTest, NETMSSAGE_OPTION_RELIABLE_IN_ORDER);

	SetHeartbeat(DEFAULT_HEARTBEAT);
}

void NetSession::Host(char const * my_id, uint16_t port, uint16_t port_range)
{
	if (m_state != SESSION_DISCONNECTED){
		//Early out with warning if you're not in the `SESSION_DISCONNECTED`
		return;
	}
	//Try to bind the socket(s).
	std::string portString = std::to_string(port);
	NetAddress hostAddr;// = new NetAddress();
	if (!AddBinding(portString.c_str(), hostAddr)){
		//On failure to bind, error and return
		SetError(SESSION_ERROR_INTERNAL, "Host not able to bind to port.");
		return;
	}

	//Create a connection for yourself (host) as index 0
	net_connection_info_t hostInfo;
	hostInfo.m_address = hostAddr; //NetAddress::GetLocal(portString.c_str());
	hostInfo.m_sessionIndex = 0;
	memcpy(hostInfo.m_id, my_id, MAX_CONN_ID_LENGTH);
	m_hostConnection = CreateConnection(hostInfo);
	m_myConnection = m_hostConnection;
	m_hostConnection->m_state = CONNECTION_READY;
	m_state = SESSION_READY;

}

void NetSession::Join(char const * my_id, net_connection_info_t const & host_info)
{
	int port = host_info.m_address.m_port;

	m_joinRequestTimer = StopWatch();
	m_joinRequestTimer.SetTimer(JOIN_RESEND_TIME);
	m_joinTimeoutTimer.SetTimer(JOIN_TIMEOUT_TIME);

	//bind the socket (starting at host's port w/range)
	int portRange = MAX_CONNECTIONS;
	int offset = 1;
	int myPort = port +offset;
	NetAddress myAddr;// = new NetAddress();
	bool bound = AddBinding(std::to_string(myPort).c_str(), myAddr);
	while (!bound && offset < portRange){
		offset++;
		myPort = port + offset;
		bound = AddBinding(std::to_string(myPort).c_str(), myAddr);
		//On failure to bind, error and return
		//SetError(SESSION_ERROR_INTERNAL, "Host not able to bind to port.");
		//return;
	}

	if (!bound){
		SetError(SESSION_ERROR_JOIN_DENIED_FULL, "No available sockets for join.");
		return;
	}

	//Create a connection for the host, bind it, and mark it as connected
	m_hostConnection = CreateConnection(host_info);
	m_hostConnection->m_state = CONNECTION_CONNECTED;

	NetMessage* joinReq = new NetMessage("join_request");
	m_hostConnection->Send(joinReq);

	net_connection_info_t myInfo;
	myInfo.m_address = myAddr;
	myInfo.m_sessionIndex = INVALID_CONNECTION_INDEX;
	m_myConnection = CreateConnection(myInfo);

	m_state = SESSION_CONNECTING;

}

void NetSession::Disconnect()
{
}

void NetSession::SetError(eSessionError error, char const * str)
{
	m_errorCode = error;
	m_errorString = str;
}

void NetSession::ClearError()
{
	m_errorCode = SESSION_OK;
	m_errorString = "No error.";
}

eSessionError NetSession::GetLastError(std::string * out_str)
{
	*out_str = m_errorString;
	eSessionError lastError = m_errorCode;
	ClearError();
	return lastError;
}

unsigned int NetSession::GetNetTimeMS()
{
	if (m_myConnection == m_hostConnection){
		return (unsigned int) (GetMasterClock()->GetCurrentSeconds() * 1000.f);
	} else {
		return m_currentClientTimeMS;
	}

	return 0;
}

void NetSession::Update()
{
	if (m_socket != nullptr){
		//do clock shit if you aren't the host
		unsigned int dt = (unsigned int) (GetMasterClock()->GetDeltaSeconds() * 1000.f);
		if (m_myConnection != m_hostConnection){
			m_desiredClientTimeMS += dt;		//this is the host time adjusted for RTT
			unsigned int dtp;
			if (m_currentClientTimeMS + dt > m_desiredClientTimeMS){
				//dtp is scaled down to 1 - MAX_NET_TIME_DILATION (.9)
				dtp = (unsigned int) ((float) dt * (1.f - MAX_NET_TIME_DILATION));
				m_clockBehavior = "-";
			} else if (m_currentClientTimeMS + dt < m_desiredClientTimeMS){
				dtp = (unsigned int) ((float) dt * (1.f + MAX_NET_TIME_DILATION));
				m_clockBehavior = "+";
			} else {
				dtp = dt;
				m_clockBehavior = "=";
			}

			m_currentClientTimeMS += dtp;
		} else {
			m_currentClientTimeMS = GetMasterClock()->GetCurrentMilliseconds();
		}


		ProcessIncoming();
		ProcessPackets();
		UpdateSession();
		UpdateConnections();
		ProcessOutgoing();

		DestroyDisconnects();
	}
}

void NetSession::UpdateSession()
{
	switch (m_state)
	{
		case (SESSION_CONNECTING):
			UpdateConnecting();
			break;
		case (SESSION_JOINING):
			UpdateJoining();
			break;
	}
}

void NetSession::UpdateConnecting()
{
	if (m_myConnection->IsConnected()){
		m_state = SESSION_JOINING;
	} else {
		if (m_joinRequestTimer.CheckAndReset()){
			NetMessage* joinMsg = new NetMessage("join_request");
			m_hostConnection->Send(joinMsg);
		} 
		if (m_joinTimeoutTimer.HasElapsed()){
			SetError(SESSION_ERROR_JOIN_DENIED, "Join request timed out");
			Disconnect();
		}
	}
}
void NetSession::UpdateJoining()
{
	//State does nothing - waiting until your connection is marked as ready in OnJoinFinished
	if (m_myConnection->IsReady()){
		m_state = SESSION_READY;
	}
}

NetConnection * NetSession::CreateConnection(net_connection_info_t const & info)
{
	NetConnection* newConnection = new NetConnection(this, info);
	newConnection->SetHeartbeat(m_sessionHeartbeat);
	newConnection->SetSendRate(m_sessionRateHz);
	//int index = GetNextConnectionIndex();
	//newConnection->m_indexInSession = index;
	m_allConnections.push_back(newConnection);

	if (info.m_sessionIndex != INVALID_CONNECTION_INDEX){		//if index is valid
		BindConnection(m_boundConnections.size(), newConnection);
		//m_boundConnections.push_back(newConnection);
	}

	return newConnection;
}

void NetSession::DestroyConnection(NetConnection * cp)
{
	ConsolePrintf(RGBA::RED, "Removing connection %i", cp->m_info.m_sessionIndex);
	//clear convenience pointers if necessary
	if (cp == m_hostConnection)
	{
		m_hostConnection = nullptr;
	}
	if (cp == m_myConnection)
	{
		m_myConnection = nullptr;
	}

	//remove bound connection if it's in there
	for(int i = m_boundConnections.size() - 1; i >= 0; i--)
	{
		if (m_boundConnections[i] == cp){
			RemoveAtFast(m_boundConnections, i);
		}
	}

	//remove from the all connections list
	for(int i = m_allConnections.size() - 1; i >= 0; i--)
	{
		if (m_allConnections[i] == cp){
			RemoveAtFast(m_allConnections, i);
		}
	}
	

	//remove any timestamped packets from this connection
	for (int i = m_timeStampedPackets.size() - 1; i >= 0; i--){
		if (m_timeStampedPackets[i]->m_fromConnection == cp){
			time_stamped_packet_t* packet = m_timeStampedPackets[i];
			RemoveAtFast(m_timeStampedPackets, i);
			delete packet;
		}
	}

	//delete
	delete cp;
	cp = nullptr;
}

void NetSession::BindConnection(uint8_t idx, NetConnection * cp)
{
	cp->m_info.m_sessionIndex = idx;
	cp->m_state = CONNECTION_CONNECTED;
	m_boundConnections.push_back(cp);
}

void NetSession::RenderInfo(AABB2 screenBounds, Renderer* renderer)
{
	//set up ya values
	float headerFontHeight = SESSION_FONT_HEADER_SIZE * screenBounds.GetWidth();
	float contentFontHeight = SESSION_FONT_CONTENT_SIZE * screenBounds.GetWidth();
	float textLineHeight = headerFontHeight* SESSION_LINE_MULTIPLIER;
	float padding = headerFontHeight * .5f;
	float yHeight = screenBounds.GetHeight() - textLineHeight;
	Vector2 newLine	= Vector2(0.f, -textLineHeight);
	Vector2 tab		= Vector2(headerFontHeight, 0.f);
	AABB2 textBox = AABB2(screenBounds.mins + Vector2(0.f, yHeight), screenBounds.maxs);
	textBox.AddPaddingToSides(-padding, 0.f);
	textBox.Translate(Vector2(padding, -padding));

	//Render Session Info
	renderer->DrawTextInBox2D("Session Info", textBox, Vector2::TOP_LEFT, headerFontHeight, TEXT_DRAW_SHRINK_TO_FIT);
	//renderer->DrawAABB2Outline(textBox, RGBA::RED);
	textBox.Translate(newLine);
	//Render net rate, Simulated latency, & simulated lag
	std::string netInfo = Stringf("Rate: %.2fhz  sim lag: %sms  sim loss: %3.2f heartRate: %3.2f  NetTime (ms): %3.2f (%s)", m_sessionRateHz, m_simulatedLatency.ToString().c_str(),  m_simulatedLoss, m_sessionHeartbeat, ((float) GetNetTimeMS()) * .001f, m_clockBehavior.c_str());
	renderer->DrawTextInBox2D(netInfo, textBox, Vector2::TOP_LEFT, contentFontHeight, TEXT_DRAW_SHRINK_TO_FIT, RGBA::LIGHTGRAY);
	textBox.Translate(newLine);

	//render socket addresses
	textBox.Translate(tab);
	renderer->DrawTextInBox2D("Socket Address...", textBox, Vector2::TOP_LEFT, headerFontHeight, TEXT_DRAW_SHRINK_TO_FIT);
	textBox.Translate(newLine);
	textBox.Translate(tab);
	renderer->DrawTextInBox2D(m_socket->GetAddressAsString(),textBox, Vector2::TOP_LEFT, contentFontHeight, TEXT_DRAW_SHRINK_TO_FIT, RGBA::LIGHTGRAY );
	textBox.Translate(newLine);
	textBox.Translate(tab * -1.f);

	//render connections
	renderer->DrawTextInBox2D("Connections...", textBox, Vector2::TOP_LEFT, headerFontHeight, TEXT_DRAW_SHRINK_TO_FIT);
	textBox.Translate(newLine);
	textBox.Translate(tab);
	std::string header = Stringf( "%-3s %-5s %-20s %-9s %-7s %-12s %-12s %-11s %-8s %-10s %-30s", 
		"--",			//3
		"idx",			//5
		"address",		//20(?)
		"rtt(ms)",		//9
		"loss%",		//7
		"lastrcv(s)",	//12
		"lastsnt(s)",	//12
		"storedREL",	//11
		"sntack",		//8
		"rcvdack",		//9
		"rcvbits");		//30
	renderer->DrawTextInBox2D(header, textBox, Vector2::TOP_LEFT, contentFontHeight, TEXT_DRAW_OVERRUN, RGBA::LIGHTGRAY);
	textBox.Translate(newLine);

	//render each BOUND connection info
	for(int i = 0; i < m_boundConnections.size(); i++){
		if (m_boundConnections[i] != nullptr){
			NetConnection* connection = m_boundConnections[i];
			std::string localString = "  ";
			if (connection->IsHost())
			{
				localString = "H";
			}
			if (connection->IsMe()){
				localString += "L";
			}

			std::string connectionText = Stringf( "%-4s %-5i %-20s %-9i %-7.3f %-12.3f %-12.3f %-11i %-8i %-10i %-30s", 
				localString.c_str(),								//3
				connection->GetConnectionIndex(),													//5
				connection->GetAddress().ToString().c_str(),		//20(?)
				connection->GetRTT(),								//9 (6.3)
				connection->GetLossRate(),							//7 (4.3)
				connection->GetLastReceivedTimeSeconds(),			//12 (9.3)
				connection->GetLastSendTimeSeconds(),				//12 (9.3)
				(int) connection->m_unconfirmedReliableMessages.size(),	//11
				connection->GetNextSentAck(),						//8 (8)
				connection->GetLastReceivedAck(),					//10 (10)
				ToBitString(connection->GetAckBitfield()).c_str());	//30 
			renderer->DrawTextInBox2D(connectionText, textBox, Vector2::TOP_LEFT, contentFontHeight, TEXT_DRAW_OVERRUN, RGBA::WHITE);
			textBox.Translate(newLine);
		}
	}

}

void NetSession::SendDirect(NetMessage * message, NetConnection * invalidConnection)
{
	message->SetDefinition(GetRegisteredMessageByName(message->m_msgName));
	message->WriteHeader();
	//invalidConnection->Send(message);

	NetPacket* packet = new NetPacket();
	packet->AdvanceWriteHead(sizeof(packet_header_t));
	if (!packet->WriteMessage(*message)){
		ConsolePrintf(RGBA::RED, "Invalid message in SendDirect - too big for a packet :(");
		return;
	}
	//send packet to connection
	packet_header_t packetHeader;
	packetHeader.m_messageCount = 1U;
	packetHeader.m_senderConnectionIndex = (uint8_t) GetLocalConnectionIndex();
	packetHeader.m_ack = invalidConnection->GetNextSentAck();
	packetHeader.m_lastReceivedAck = invalidConnection->GetLastReceivedAck();
	packetHeader.m_previousReceivedAckBitfield = invalidConnection->GetAckBitfield();

	//// get the tracker slot for this ack being sent out; 
	//TrackedPacket *tracker = AddTrackedPacket( header.m_ack );
	//tracker;  // unused for now - will track reliables soon; 
	PacketTracker* tracker = invalidConnection->AddTrackedPacketOnSend(packetHeader);
	tracker;
	//
	packet->WriteHeader(packetHeader);

	if (m_socket->SendTo(invalidConnection->GetAddress(), packet->GetBuffer(), packet->GetWrittenByteCount()) == 0U){
		ConsolePrintf(RGBA::RED, "Invalid connection for DirectSend");
	} else {
		invalidConnection->IncrementSendAck();
		invalidConnection->MarkSendTime();
	}

	delete packet;
	//delete invalidConnection;		//we can delete connection because it's just for one message...right?
}

void NetSession::SetHeartbeat(float hz)
{
	m_sessionHeartbeat = hz;
	for(NetConnection* connection : m_boundConnections){
		if (connection != nullptr){
			connection->SetHeartbeat(m_sessionHeartbeat);
		}
	}
}

void NetSession::SetSessionSendRate(float hz)
{
	m_sessionRateHz = hz;
	for(NetConnection* connection : m_boundConnections){
		if (connection != nullptr){
			connection->SetSendRate();		//just updates timers
		}
	}
}

void NetSession::SetConnectionSendRate(int connectionIndex, float hz)
{
	if (m_boundConnections[connectionIndex] != nullptr){
		m_boundConnections[connectionIndex]->SetSendRate(hz);
	}
}

bool NetSession::AddBinding(const char * port, NetAddress& out_addr)
{
	out_addr = NetAddress::GetLocal(port);
	if (!m_socket->Bind(out_addr, MAX_CONNECTIONS)){
		ConsolePrintf(RGBA::RED, "Could not bind session to address: %s", port);
		return false;
	} else {
		ConsolePrintf(RGBA::GREEN, "Bound session on address %s", out_addr.ToString().c_str());
		m_socket->SetUnblocking();
		return true;
	}
}

//
//NetConnection * NetSession::AddConnection(uint8_t idx, NetAddress addr)
//{
//	//NetConnection* newConnection = new NetConnection(this, idx, addr);
//	//if (newConnection->GetAddress() == m_socket->GetAddress()){
//	//	newConnection->m_isLocal = true;
//	//}
//	//m_boundConnections[idx] = newConnection;
//	//newConnection->SetHeartbeat(m_sessionHeartbeat);
//	//return newConnection;
//	return nullptr;
//}

//void NetSession::RemoveConnection(uint8_t idx)
//{
//	//if (m_connections[idx] != nullptr){
//	//	delete m_connections[idx];
//	//	m_connections[idx] = nullptr;
//	//	ConsolePrintf(RGBA::RED, "Removed connection at %i", idx);
//	//}
//}

NetConnection * NetSession::GetConnection(uint8_t idx) const
{
	for(NetConnection* conn : m_boundConnections){
		if (conn->GetConnectionIndex() == idx){
			return conn;
		}
	}
	return nullptr;
}

NetConnection * NetSession::GetConnectionByAddress(NetAddress addr) const
{
	for (int i = 0; i < m_boundConnections.size(); i++){
		if (m_boundConnections[i] != nullptr){
			if (m_boundConnections[i]->GetAddress() == addr){
				return m_boundConnections[i];
			}
		}
	}
	return nullptr;
}

NetConnection * NetSession::GetLocalConnection() const
{
	for (NetConnection* connection : m_boundConnections){
		if (connection != nullptr){
			if (connection->IsMe()){
				return connection;
			}
		}
	}
	return nullptr;
}

void NetSession::ProcessHostTime(unsigned int hostTimeMS)
{
	if ((hostTimeMS < m_lastReceivedHostTimeMS)){
		//early out - don't process
		return;
	} else {
		//if it is more recent than last time update, process it.
		m_lastReceivedHostTimeMS = hostTimeMS + (m_hostConnection->GetRTT() * .5f);
		m_desiredClientTimeMS = m_lastReceivedHostTimeMS;
		
	}
}

unsigned int NetSession::GetLocalConnectionIndex() const
{
	for (unsigned int i = 0; i < m_boundConnections.size(); i++){
		if (m_boundConnections[i] != nullptr){
			if (m_boundConnections[i]->IsMe()){
				return i;
			}
		}
	}
	return (unsigned int) -1;
}

float NetSession::GetSessionSendRateHZ() const
{
	return m_sessionRateHz;
}

void NetSession::SetSimulatedLoss(float lossAmount)
{
	m_simulatedLoss = lossAmount;
}

void NetSession::SetSimulatedLatency(int minAddedLatencyMS, int maxAddedLatencyMS)
{
	if (maxAddedLatencyMS < minAddedLatencyMS){
		maxAddedLatencyMS = minAddedLatencyMS;
	}
	m_simulatedLatency = IntRange(minAddedLatencyMS, maxAddedLatencyMS);
}

void NetSession::RegisterMessage(int msgIndex, std::string name, NetSessionMessageCB messageCB, eNetMessageOptions messageOptions, uint8_t msgChannelIndex)
{
	net_message_definition_t* messageDef = new net_message_definition_t();
	messageDef->m_messageCB = messageCB;
	messageDef->m_messageName = name;
	messageDef->m_messageOptions = messageOptions;
	messageDef->m_fixedIndex = msgIndex;
	messageDef->m_channelID = msgChannelIndex;
	if (msgIndex == -1){
		messageDef->m_messageID = (uint8_t) m_registeredMessages.size();
		m_unassignedMessages.push_back(messageDef);
	} else {
		messageDef->m_messageID = msgIndex;
		m_fixedIndexMessages.push_back(messageDef);
	}
}

void NetSession::SortMessageIDs()
{
	int numMessages = m_unassignedMessages.size() + m_fixedIndexMessages.size();
	std::sort(m_unassignedMessages.begin(), m_unassignedMessages.end(), CompareDefinitionsByName);
	m_registeredMessages.resize(numMessages);
	int unassignedIndex = 0;
	for (int i = 0; i < numMessages; i++)
	{
		bool addedFixedMessage = false;
		for (net_message_definition_t* fixedIndexMsg : m_fixedIndexMessages){
			if (fixedIndexMsg->m_fixedIndex == i){
				m_registeredMessages[i] = fixedIndexMsg;
				addedFixedMessage = true;
			}
		}
		if (!addedFixedMessage){
			m_registeredMessages[i] = m_unassignedMessages[unassignedIndex];
			unassignedIndex++;
		}
		m_registeredMessages[i]->m_messageID = (uint8_t) i;
	}
	int x = 0;
}

bool NetSession::IsMessageRegistered(std::string name)
{
	for (int i = 0; i < m_registeredMessages.size(); i++){
		if (m_registeredMessages[i]->m_messageName == name){
			return true;
		}
	}
	return false;
}

bool NetSession::IsMessageRegistered(uint16_t id)
{
	for (int i = 0; i < m_registeredMessages.size(); i++){
		if (m_registeredMessages[i]->m_messageID == id){
			return true;
		}
	}
	return false;
}

net_message_definition_t* NetSession::GetRegisteredMessageByName(std::string name)
{
	for (int i = 0; i < m_registeredMessages.size(); i++){
		if (m_registeredMessages[i]->m_messageName == name){
			return m_registeredMessages[i];
		}
	}
	return nullptr;
}

net_message_definition_t * NetSession::GetRegisteredMessageByID(uint8_t id)
{
	for (int i = 0; i < m_registeredMessages.size(); i++){
		if (m_registeredMessages[i]->m_messageID == id){
			return m_registeredMessages[i];
		}
	}
	return nullptr;
}

void NetSession::UpdateConnections()
{
	for (NetConnection* connection : m_boundConnections){
		if (connection != nullptr){
			connection->Update();
		}
	}
}

void NetSession::ProcessIncoming()
{

	//see if you received anything
	byte_t buffer[PACKET_MTU]; 
	NetAddress from_addr; 
	size_t read = m_socket->ReceiveFrom( &from_addr, buffer, PACKET_MTU ); 
	
	
	//once you have a message, fill a BytePacker (NetMessage) like we did w/RCS (?)
	if (read > 0U){
		time_stamped_packet_t* timedPacket = new time_stamped_packet_t();
		NetConnection* from = GetConnectionByAddress(from_addr);
		if (from == nullptr){
			//if the connection doesn't exist, make a pretend one for processing
			from = new NetConnection(this, INVALID_CONNECTION_INDEX, from_addr);
		}
		if (from != nullptr){
			if (CheckRandomChance(m_simulatedLoss)){
				//ConsolePrintf(RGBA::RED, "Packet lost.");
				return;
			}



			timedPacket->m_packet->WriteBytes(read, buffer);
			timedPacket->m_fromConnection = from;
			timedPacket->AddSimulatedLatency(m_simulatedLatency);

			m_timeStampedPackets.push_back(timedPacket);

			std::sort(m_timeStampedPackets.begin(), m_timeStampedPackets.end(), TimeStampedPacketCompare);

			//NetPacket* packet = new NetPacket();

			//NetMessage message;
			//get the size of the entire buffer with the same method we used for RCS
			//packet->WriteBytes(read, buffer);		//writes the rest of the buffer to the NetMessage packer
			
			
		}
	}
}

void NetSession::ProcessOutgoing()
{
	// for every net conneciton, see if it has any messages to send
	// and send all messages queued during the frame using your socket
	// basically queueing all messages for a frame and then sending it?
	for (int i = 0; i < m_boundConnections.size(); i++){
		if (m_boundConnections[i] != nullptr && m_boundConnections[i]->IsConnected()){
			if (m_boundConnections[i]->CanSendToConnection()){
				SendPacketsForConnection(i);
			}
		}
	}
}

void NetSession::DestroyDisconnects()
{
	//remove any disconnects from this frame
	for (int i = m_boundConnections.size() - 1; i >= 0; i--){
		if (m_boundConnections[i]->IsDisconnected()){
			DestroyConnection(m_boundConnections[i]);
		}
	}

	//if you or your host disconnect, remove all connections and disconnect
	if (m_hostConnection == nullptr || m_myConnection == nullptr){
		for (int i = m_boundConnections.size() - 1; i >= 0; i--){
			DestroyConnection(m_boundConnections[i]);
		}

		m_state = SESSION_DISCONNECTED;
	}
}

void NetSession::ProcessMessage(NetMessage* message, NetConnection* from)
{
	// figure out which kind of message it is
	// seems like the name is encoded as the first string?
	uint8_t messageType;
	message->Read(&messageType, false);
	bool shouldDelete = true;
	

	//get the appropriate callback from the map of registered callbacks
	if (IsMessageRegistered(messageType)){
		net_message_definition_t* definition = GetRegisteredMessageByID(messageType);
		if (messageType != 0){
			int x = 0;

		}
		if (definition== nullptr){
			LogErrorf("No definition found for message index: %i", message->m_msgID);
		} else {
			message->m_definition = definition;
		}

		//if the definition requires a connection and we don't have a valid connection, chuck it
		if (!definition->IsConnectionless() && !from->IsValidConnection()){
			//discard message and log warning
			LogTaggedPrintf("net", "No connection found for incoming message that requires connection. (%s)", message->m_definition->m_messageName.c_str());
		} else {
			//process as usual
			if (definition->IsReliable()){
				uint16_t reliableID;
				message->Read(&reliableID);
				message->m_reliableID = reliableID;
				if (from->HasReceivedReliable(reliableID)){
					//you have already processed this message - do nothing
					return;
				} else {
					if (definition->IsInOrder()){
						uint16_t seqID;
						message->Read(&seqID);
						message->m_sequenceID = seqID;
						if (from->IsMessageNextExpectedInSequence(message)){
							//process the message and then add received reliable to the connection
							((definition->m_messageCB))( *message, net_sender_t(from));
							//ConsolePrintf(RGBA::YELLOW, "Processing message %i", message->m_sequenceID);
							from->AddReceivedReliable(reliableID);
							from->AddReceivedInOrderMessage(message);
							from->ProcessOutOfOrderMessagesForChannel(definition->m_channelID);
							//ConsolePrintf(RGBA::YELLOW, "Finished processing message %i", message->m_sequenceID);
						} else {
							//ConsolePrintf("Received out of order message %i", message->m_sequenceID);
							from->AddOutOfOrderMessage(message);
							shouldDelete = false;
							//from->AddReceivedReliable(reliableID);
						}
					} else {
						//process the message and then add received reliable to the connection
						((definition->m_messageCB))( *message, net_sender_t(from));
						from->AddReceivedReliable(reliableID);
					}

				}
			} else {
				//process unreliable message as usual
				((definition->m_messageCB))( *message, net_sender_t(from));
			}
		}

		//if (!from->IsValidConnection()){
		//	//clean up "from" connection if it was made on the fly
		//	delete from;
		//}
		
	}
	if (shouldDelete){
		delete (message);
	}
}



void NetSession::SendPacketsForConnection(unsigned int connectionIndex)
{
	NetPacket* packet = new NetPacket();
	packet->AdvanceWriteHead(sizeof(packet_header_t));
	uint8_t messagesSent = 0;
	NetConnection* connection = m_boundConnections[connectionIndex];

	std::vector<NetMessage*> reliablesInPacket = std::vector<NetMessage*>();

	//send unconfirmed reliable messages
	for (NetMessage* msg : connection->m_unconfirmedReliableMessages){
		if (connection->ShouldSendReliableMessage(msg)){
			if (reliablesInPacket.size() < MAX_RELIABLES_PER_PACKET){
				if (packet->WriteMessage(*msg)){
					messagesSent++;
					msg->ResetAge();
					reliablesInPacket.push_back(msg);
				} else {
					SendPacketToConnection((uint8_t) connectionIndex, packet, messagesSent, reliablesInPacket);
					messagesSent = 0;
					delete packet;
					reliablesInPacket = { msg };
					packet = new NetPacket();
					packet->WriteMessage(*msg);		//this is always guaranteed to be smaller than net packet size
				}
			} else {
				SendPacketToConnection((uint8_t) connectionIndex, packet, messagesSent, reliablesInPacket);
				messagesSent = 0;
				delete packet;
				reliablesInPacket = { msg };
				packet = new NetPacket();
				packet->WriteMessage(*msg);		//this is always guaranteed to be smaller than net packet size
			}
		}
	}

	//write unsent reliables
	for (NetMessage* msg : connection->m_unsentReliableMessages)
	{
		if (msg != nullptr){
			if (connection->CanSendNewReliable()){
				if (packet->HasRoomForMessage(msg)){
					if (reliablesInPacket.size() < MAX_RELIABLES_PER_PACKET){
						msg->m_reliableID = connection->GetAndIncrementNextReliableID();
						msg->WriteHeader();
						ASSERT_OR_DIE(packet->WriteMessage(*msg), "Packet ran out of space after check for space??");
						messagesSent++;
						//move from unsent list to unconfirmed list
						connection->MarkMessageAsSentForFirstTime(msg);
						reliablesInPacket.push_back(msg);
					} else {
						SendPacketToConnection((uint8_t) connectionIndex, packet, messagesSent, reliablesInPacket);
						messagesSent = 0;
						delete packet;
						reliablesInPacket = { msg };
						packet = new NetPacket();
						packet->WriteMessage(*msg);		//this is always guaranteed to be smaller than net packet size
					}
				} else {
					SendPacketToConnection((uint8_t) connectionIndex, packet, messagesSent, reliablesInPacket);
					messagesSent = 0;
					delete packet;
					reliablesInPacket = { msg };
					packet = new NetPacket();
					packet->WriteMessage(*msg);		//this is always guaranteed to be smaller than net packet size
				}
			}
		}
	}

	for(NetMessage* msg : connection->m_unsentUnreliableMessages){
		if (packet->WriteMessage(*msg)){
			messagesSent++;
		} else {
			SendPacketToConnection((uint8_t) connectionIndex, packet, messagesSent, reliablesInPacket);
			messagesSent = 0;
			delete packet;
			reliablesInPacket = { msg };
			packet = new NetPacket();
			packet->WriteMessage(*msg);		//this is always guaranteed to be smaller than net packet size
		}
	}

	// ooold
	//for (int msgNum = 0; msgNum < m_connections[connectionIndex]->m_unsentUnreliableMessages.size(); msgNum++){
	//	NetMessage* msg = m_connections[connectionIndex]->m_unsentUnreliableMessages[msgNum];

	//	// write the message to the net packet
	//	// if you don't have the space to write a packet, send existing packet,
	//	// and start writing a new packet
	//	if (!packet->WriteMessage(*msg)){
	//		SendPacketToConnection((uint8_t) connectionIndex, packet, messagesSent);
	//		messagesSent = 0;
	//		delete packet;
	//		packet = new NetPacket();
	//		packet->WriteMessage(*msg);		//this is always guaranteed to be smaller than net packet size
	//	}
	//	messagesSent++;
	//	//m_socket->SendTo(connection->GetAddress(), msg->GetBuffer(), msg->GetWrittenByteCount());
	//}
	if (messagesSent > 0){
		SendPacketToConnection((uint8_t) connectionIndex, packet, messagesSent, reliablesInPacket);
	}
	delete packet;
	//if we didn't remove the connection after a disconnect, clear the connection's message queue
	if (m_boundConnections[connectionIndex] != nullptr){
		m_boundConnections[connectionIndex]->ClearMessageQueue();
	}
}

void NetSession::ProcessPackets()
{
	for (size_t i = 0; i < m_timeStampedPackets.size(); i++){
		time_stamped_packet_t* tsPacket = m_timeStampedPackets[i];
		if (tsPacket->m_timeToProcess < GetCurrentTimeSeconds()){
			//process a packet
			packet_header_t packetHeader;

			bool wasTrackedAndUnconfirmed = false;
			//confirm packet received on the net connection
			tsPacket->m_packet->ReadHeader(packetHeader);
			if (tsPacket->m_fromConnection != nullptr){
				tsPacket->m_fromConnection->MarkReceiveTime();
				tsPacket->m_fromConnection->UpdateReceivedAcks(packetHeader.m_ack);
				wasTrackedAndUnconfirmed = tsPacket->m_fromConnection->ConfirmPacketReceived(packetHeader.m_lastReceivedAck);
			}
			//go through the packet and process each message
			for (unsigned int j = 0; j < packetHeader.m_messageCount; j++){
				NetMessage* message =  new NetMessage();
				bool read = tsPacket->m_packet->ReadMessage(message);
				if (read) 
				{
					ProcessMessage(message, tsPacket->m_fromConnection);
				} else {
					delete message;
					break;
				}
				//delete message;
			}

			m_timeStampedPackets[i] = nullptr;
			delete tsPacket;
		} else {
			break;
		}
	}

	for (int i = (int) m_timeStampedPackets.size()-1; i >= 0; i--){
		if (m_timeStampedPackets[i] == nullptr){
			RemoveAtFast(m_timeStampedPackets, i);
		}
	}
}

void NetSession::SendPacketToConnection(uint8_t connIndex, NetPacket * packet, uint8_t numMessages, std::vector<NetMessage*> reliablesInPacket)
{
	NetConnection* connToSendTo = m_boundConnections[connIndex];
	packet_header_t packetHeader;
	packetHeader.m_messageCount = numMessages;
	packetHeader.m_senderConnectionIndex = (uint8_t) GetLocalConnectionIndex();
	packetHeader.m_ack = connToSendTo->GetNextSentAck();
	packetHeader.m_lastReceivedAck = connToSendTo->GetLastReceivedAck();
	packetHeader.m_previousReceivedAckBitfield = connToSendTo->GetAckBitfield();

	//// get the tracker slot for this ack being sent out; 
	//TrackedPacket *tracker = AddTrackedPacket( header.m_ack );
	//tracker;  // unused for now - will track reliables soon; 
	PacketTracker* tracker = connToSendTo->AddTrackedPacketOnSend(packetHeader);
	for (NetMessage* reliable : reliablesInPacket){
		tracker->AddTrackedReliable(reliable);
	}
	//
	packet->WriteHeader(packetHeader);

	if (m_socket->SendTo(connToSendTo->GetAddress(), packet->GetBuffer(), packet->GetWrittenByteCount()) == 0U){
		//if you can't send, remove the connection
		connToSendTo->Disconnect();
	} else {
		connToSendTo->IncrementSendAck();
		connToSendTo->MarkSendTime();
	}
}

bool TimeStampedPacketCompare(time_stamped_packet_t * leftHandSide, time_stamped_packet_t * rightHandSide)
{
	return leftHandSide->m_timeToProcess < rightHandSide->m_timeToProcess;
}

bool OnHeartbeat(NetMessage msg, net_sender_t const & from)
{
	//UNUSED(from);
	//ConsolePrintf(RGBA::GREEN.GetColorWithAlpha(165), "Heartbeat from connection %s", from.m_connection->GetAddress().ToString().c_str()); 
	NetSession* session = from.m_connection->m_owningSession;
	//if (!session->m_myConnection->IsHost()){
		unsigned int hostTime;
		msg.Read(&hostTime);
		//ConsolePrintf("Receiving host time: %i", hostTime);
		session->ProcessHostTime(hostTime);
	//}
	return true;
}

bool OnPing( NetMessage msg, net_sender_t const &from ) 
{
	std::string str; 
	msg.ReadString( str ); 

	ConsolePrintf( "Received ping from %s: %s", 
		from.m_connection->GetAddress().ToString().c_str(), 
		str.c_str() ); 

	// ping responds with pong
	NetMessage* pong = new NetMessage( "pong" ); 
	//pong->WriteData( str ); 
	from.m_connection->m_owningSession->SendDirect(pong, from.m_connection);

	// all messages serve double duty
	// do some work, and also validate
	// if a message ends up being malformed, we return false
	// to notify the session we may want to kick this connection; 
	return true; 
}

bool OnPong( NetMessage msg, net_sender_t const &from ) 
{
	//std::string str; 
	//msg.ReadString( str ); 

	ConsolePrintf( "Received pong from %s", 
		from.m_connection->GetAddress().ToString().c_str()); 

	// all messages serve double duty
	// do some work, and also validate
	// if a message ends up being malformed, we return false
	// to notify the session we may want to kick this connection; 
	return true; 
}

bool OnJoinRequest(NetMessage msg, net_sender_t const & from)
{
	NetSession* session = from.m_connection->m_owningSession;
	bool deny = false;
	bool ignore = false;
	//deny if you aren't the host
	if (!session->m_myConnection->IsHost()){
		deny = true;
	}
	//deny if you don't have space
	if (session->m_boundConnections.size() >= MAX_CONNECTIONS){
		deny = true;
	}
	//ignore if you already have a join here
	if (session->GetConnectionByAddress(from.m_connection->GetAddress()) != nullptr){
		ignore = true;
	}
	if (!deny && !ignore){
		//try to make a new connection
		NetConnection* newConnection = session->CreateConnection(from.m_connection->m_info);
		//try to assign an index & bind the connection
		bool assigned = false;
		for (uint8_t i = 1; i < MAX_CONNECTIONS; i++){
			if (session->GetConnection(i) == nullptr){
				session->BindConnection(i, newConnection);
				assigned=true;
				break;
			}
		}
		//deny if you can't bind a connection
		if (!assigned){
			deny = true;
			if (newConnection != nullptr){
				delete newConnection;
			}
		} else {
			ConsolePrintf(RGBA::GREEN, "Join request from %s accepted", newConnection->GetAddress().ToString().c_str());
			//send join_accept
			NetMessage* acceptMessage = new NetMessage( "join_accept", session ); 
			//write accept data - the index they were assigned
			acceptMessage->Write(newConnection->m_info.m_sessionIndex);
			acceptMessage->IncrementMessageSize(sizeof(newConnection->m_info.m_sessionIndex));
			acceptMessage->Write(session->m_currentClientTimeMS);
			acceptMessage->IncrementMessageSize(sizeof(unsigned int));
			newConnection->Send(acceptMessage);

			//send a heartbeat with your current time
			NetMessage* heartbeat = new NetMessage("heartbeat");
			heartbeat->Write(session->m_currentClientTimeMS);
			heartbeat->IncrementMessageSize(sizeof(unsigned int));
			newConnection->Send(heartbeat);
			//send join_finished (optional)
		}
	}

	if (deny)
	{
		ConsolePrintf(RGBA::RED, "Join request from %s denied", from.m_connection->GetAddress().ToString().c_str());
		NetMessage* denyMessage = new NetMessage( "join_deny" ); 
		//write deny data
		from.m_connection->m_owningSession->SendDirect(denyMessage, from.m_connection);
	}

	return true;
}

bool OnJoinDeny(NetMessage msg, net_sender_t const & from)
{
	return false;
}

bool OnJoinAccept(NetMessage msg, net_sender_t const & from)
{
	
	NetSession* session = from.m_connection->m_owningSession;
	uint8_t sessionIndex;
	msg.Read(&sessionIndex);
	ConsolePrintf(RGBA::GREEN, "Join accepted. Adding connection at index %i", sessionIndex);
	session->BindConnection(sessionIndex, session->m_myConnection);
	unsigned int hostMS;
	msg.Read(&hostMS);
	//session->ProcessHostTime(session->m_desiredClientTimeMS);
	session->m_currentClientTimeMS = hostMS;
	session->m_lastReceivedHostTimeMS = hostMS;
	ConsolePrintf("Setting time to: %i", session->m_currentClientTimeMS);
	return true;
}

bool OnNewConnection(NetMessage msg, net_sender_t const & from)
{
	return false;
}

bool OnJoinFinished(NetMessage msg, net_sender_t const & from)
{
	return false;
}

bool OnUpdateConnectionState(NetMessage msg, net_sender_t const & from)
{
	//eConnectionState newState;
	uint8_t indx;
	uint8_t stateUint;
	msg.Read(&indx);
	msg.Read(&stateUint);
	NetConnection* conn = from.m_connection->m_owningSession->GetConnection(indx);
	//if (conn != nullptr){	
	//	ConsolePrintf("Setting connection %i to state %i (from conn %i)", indx, stateUint, from.m_connection->m_info.m_sessionIndex );
	//	from.m_connection->m_owningSession->GetConnection(indx)->m_state = (eConnectionState) stateUint;
	//} else {
	//	ConsolePrintf("No connection at %i! I don't care about state change.", indx);
	//}
	//
	//from.m_connection->m_state = (eConnectionState) stateUint;
	return true;
}

bool OnHangup(NetMessage msg, net_sender_t const & from)
{
	from.m_connection->Disconnect();
	ConsolePrintf("Hangup from %i", from.m_connection->GetConnectionIndex());
	return true;
}

bool OnNetMsgUnreliableTest(NetMessage msg, net_sender_t const & from)
{
	UNUSED(from);
	int numSent;
	int numToSend;

	if (!msg.Read(&numSent) || !msg.Read(&numToSend)){
		return false;
	}
	ConsolePrintf("Received unreliable %i of %i from %s", numSent, numToSend, from.m_connection->GetAddress().ToString().c_str());
	return true;
}

bool OnNetMsgReliableTest(NetMessage msg, net_sender_t const & from)
{
	UNUSED(from);
	int numSent;
	int numToSend;

	if (!msg.Read(&numSent) || !msg.Read(&numToSend)){
		return false;
	}
	ConsolePrintf("Received reliable %i of %i from %s", numSent, numToSend, from.m_connection->GetAddress().ToString().c_str());
	return true;
}

bool OnNetMsgSequenceTest(NetMessage msg, net_sender_t const & from)
{
	UNUSED(from);
	int numSent;
	int numToSend;

	if (!msg.Read(&numSent) || !msg.Read(&numToSend)){
		return false;
	}
	ConsolePrintf("Received reliable in-order %i of %i from %s", numSent, numToSend, from.m_connection->GetAddress().ToString().c_str());
	return true;
}

time_stamped_packet_t::time_stamped_packet_t()
{
	m_packet = new NetPacket();
	m_timeToProcess = GetCurrentTimeSeconds();
}

time_stamped_packet_t::~time_stamped_packet_t()
{
	delete m_packet;
}

void time_stamped_packet_t::AddSimulatedLatency(IntRange simulatedLatencySeconds)
{
	//increment time to process by random number of ms
	m_timeToProcess += ((double) simulatedLatencySeconds.GetRandomInRange() * .001);
}

bool time_stamped_packet_t::IsValidConnection() const
{
	return m_fromConnection->IsValidConnection();
}
