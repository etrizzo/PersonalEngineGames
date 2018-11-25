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
	RegisterMessage( NETMSG_UNRELIABLE_TEST, "unreliable_test", (NetSessionMessageCB) OnNetMsgUnreliableTest, NETMESSAGE_OPTION_UNRELIABLE_REQUIRES_CONNECTION);
	RegisterMessage( NETMSG_RELIABLE_TEST, "reliable_test", (NetSessionMessageCB) OnNetMsgReliableTest, NETMESSAGE_OPTION_RELIABLE );

	SetHeartbeat(DEFAULT_HEARTBEAT);
}

void NetSession::Update()
{
	if (m_socket != nullptr){
		ProcessIncoming();
		ProcessPackets();
		UpdateConnections();
		ProcessOutgoing();
	}
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
	std::string netInfo = Stringf("Rate: %.2fhz  sim lag: %sms  sim loss: %3.2f", m_sessionRateHz, m_simulatedLatency.ToString().c_str(), m_simulatedLoss);
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

	//render each connection info
	for(int i = 0; i < MAX_CONNECTIONS; i++){
		if (m_connections[i] != nullptr){
			NetConnection* connection = m_connections[i];
			std::string localString = " ";
			if (connection->m_isLocal){
				localString = "L";
			}

			std::string connectionText = Stringf( "%-3s %-5i %-20s %-9.3f %-7.3f %-12.3f %-12.3f %-11i %-8i %-10i %-30s", 
				localString.c_str(),								//3
				i,													//5
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
	for(NetConnection* connection : m_connections){
		if (connection != nullptr){
			connection->SetHeartbeat(m_sessionHeartbeat);
		}
	}
}

void NetSession::SetSessionSendRate(float hz)
{
	m_sessionRateHz = hz;
	for(NetConnection* connection : m_connections){
		if (connection != nullptr){
			connection->SetSendRate();		//just updates timers
		}
	}
}

void NetSession::SetConnectionSendRate(int connectionIndex, float hz)
{
	if (m_connections[connectionIndex] != nullptr){
		m_connections[connectionIndex]->SetSendRate(hz);
	}
}

void NetSession::AddBinding(const char * port)
{
	NetAddress addr = NetAddress::GetLocal(port);
	if (!m_socket->Bind(addr, MAX_CONNECTIONS)){
		ConsolePrintf(RGBA::RED, "Could not bind session to address: %s", port);
	} else {
		ConsolePrintf(RGBA::GREEN, "Hosting session on address %s", addr.ToString().c_str());
		m_socket->SetUnblocking();
	}
}

NetConnection * NetSession::AddConnection(uint8_t idx, NetAddress addr)
{
	NetConnection* newConnection = new NetConnection(this, idx, addr);
	if (newConnection->m_address == m_socket->GetAddress()){
		newConnection->m_isLocal = true;
	}
	m_connections[idx] = newConnection;
	newConnection->SetHeartbeat(m_sessionHeartbeat);
	return newConnection;
}

void NetSession::RemoveConnection(uint8_t idx)
{
	if (m_connections[idx] != nullptr){
		delete m_connections[idx];
		m_connections[idx] = nullptr;
		ConsolePrintf(RGBA::RED, "Removed connection at %i", idx);
	}
}

NetConnection * NetSession::GetConnection(uint8_t idx) const
{
	return m_connections[idx];
}

NetConnection * NetSession::GetConnectionByAddress(NetAddress addr) const
{
	for (int i = 0; i < MAX_CONNECTIONS; i++){
		if (m_connections[i] != nullptr){
			if (m_connections[i]->GetAddress() == addr){
				return m_connections[i];
			}
		}
	}
	return nullptr;
}

NetConnection * NetSession::GetLocalConnection() const
{
	for (NetConnection* connection : m_connections){
		if (connection != nullptr){
			if (connection->m_isLocal){
				return connection;
			}
		}
	}
	return nullptr;
}

unsigned int NetSession::GetLocalConnectionIndex() const
{
	for (unsigned int i = 0; i < MAX_CONNECTIONS; i++){
		if (m_connections[i] != nullptr){
			if (m_connections[i]->m_isLocal){
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

void NetSession::RegisterMessage(int msgIndex, std::string name, NetSessionMessageCB messageCB, eNetMessageOptions messageOptions)
{
	net_message_definition_t* messageDef = new net_message_definition_t();
	messageDef->m_messageCB = messageCB;
	messageDef->m_messageName = name;
	messageDef->m_messageOptions = messageOptions;
	messageDef->m_fixedIndex = msgIndex;
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
	for (NetConnection* connection : m_connections){
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
	for (int i = 0; i < MAX_CONNECTIONS; i++){
		if (m_connections[i] != nullptr){
			if (m_connections[i]->CanSendToConnection()){
				SendPacketsForConnection(i);
			}
		}
	}
}

void NetSession::ProcessMessage(NetMessage message, NetConnection* from)
{
	// figure out which kind of message it is
	// seems like the name is encoded as the first string?
	uint8_t messageType;
	message.Read(&messageType, false);
	

	//get the appropriate callback from the map of registered callbacks
	if (IsMessageRegistered(messageType)){
		net_message_definition_t* definition = GetRegisteredMessageByID(messageType);
		if (definition== nullptr){
			LogErrorf("No definition found for message index: %i", message.m_msgID);
		}

		//if the definition requires a connection and we don't have a valid connection, chuck it
		if (!definition->IsConnectionless() && !from->IsValidConnection()){
			//discard message and log warning
			LogTaggedPrintf("net", "No connection found for incoming message that requires connection. (%s)", message.m_definition->m_messageName.c_str());
		} else {
			//process as usual
			if (definition->IsReliable()){
				uint16_t reliableID;
				message.Read(&reliableID);
				if (from->HasReceivedReliable(reliableID)){
					//you have already processed this message - do nothing
					return;
				} else {
					//process the message and then add received reliable to the connection
					((definition->m_messageCB))( message, net_sender_t(from));
					from->AddReceivedReliable(reliableID);
				}
			} else {
				//process unreliable message as usual
				((definition->m_messageCB))( message, net_sender_t(from));
			}
		}

		if (!from->IsValidConnection()){
			//clean up "from" connection if it was made on the fly
			delete from;
		}
	}
}



void NetSession::SendPacketsForConnection(unsigned int connectionIndex)
{
	NetPacket* packet = new NetPacket();
	packet->AdvanceWriteHead(sizeof(packet_header_t));
	uint8_t messagesSent = 0;
	NetConnection* connection = m_connections[connectionIndex];

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
					std::vector<NetMessage*> reliablesInPacket = { msg };
					packet = new NetPacket();
					packet->WriteMessage(*msg);		//this is always guaranteed to be smaller than net packet size
				}
			} else {
				SendPacketToConnection((uint8_t) connectionIndex, packet, messagesSent, reliablesInPacket);
				messagesSent = 0;
				delete packet;
				std::vector<NetMessage*> reliablesInPacket = { msg };
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
						std::vector<NetMessage*> reliablesInPacket = { msg };
						packet = new NetPacket();
						packet->WriteMessage(*msg);		//this is always guaranteed to be smaller than net packet size
					}
				} else {
					SendPacketToConnection((uint8_t) connectionIndex, packet, messagesSent, reliablesInPacket);
					messagesSent = 0;
					delete packet;
					std::vector<NetMessage*> reliablesInPacket = { msg };
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
			std::vector<NetMessage*> reliablesInPacket = { msg };
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
	if (m_connections[connectionIndex] != nullptr){
		m_connections[connectionIndex]->ClearMessageQueue();
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
				wasTrackedAndUnconfirmed = tsPacket->m_fromConnection->ConfirmPacketReceived(packetHeader.m_ack);
			}
			//go through the packet and process each message
			for (unsigned int j = 0; j < packetHeader.m_messageCount; j++){
				NetMessage message;
				tsPacket->m_packet->ReadMessage(&message);
				ProcessMessage(message, tsPacket->m_fromConnection);
				
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
	NetConnection* connToSendTo = m_connections[connIndex];
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
		RemoveConnection(connIndex);
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
	UNUSED(from);
	//ConsolePrintf(RGBA::GREEN.GetColorWithAlpha(165), "Heartbeat from connection %s", from.m_connection->GetAddress().ToString().c_str()); 
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
