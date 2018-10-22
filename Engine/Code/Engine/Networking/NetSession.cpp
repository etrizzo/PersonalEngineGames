#include "NetSession.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Networking/NetConnection.hpp"
#include "Engine/Networking/UDPSocket.hpp"
#include "Engine/Networking/NetPacket.hpp"
#include "Engine/Math/Renderer.hpp"

NetSession::NetSession()
{
	m_socket = new UDPSocket();
	Startup();
}

void NetSession::Startup()
{
	RegisterMessage("heartbeat", (NetSessionMessageCB) OnHeartbeat);

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
	std::string header = Stringf( "%-3s %-5s %-20s %-9s %-7s %-12s %-12s %-8s %-10s %-30s", 
		"--",			//3
		"idx",			//5
		"address",		//20(?)
		"rtt(ms)",		//9
		"loss%",		//7
		"lastrcv(s)",	//12
		"lastsnt(s)",	//12
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

			std::string connectionText = Stringf( "%-3s %-5i %-20s %-9.3f %-7.3f %-12.3f %-12.3f %-8i %-10i %-30s", 
				localString.c_str(),								//3
				i,													//5
				connection->GetAddress().ToString().c_str(),		//20(?)
				connection->GetRTT(),								//9 (6.3)
				connection->GetLossRate(),							//7 (4.3)
				connection->GetLastReceivedTimeSeconds(),			//12 (9.3)
				connection->GetLastSendTimeSeconds(),				//12 (9.3)
				connection->GetNextSentAck(),						//8 (8)
				connection->GetLastReceivedAck(),					//10 (10)
				ToBitString(connection->GetAckBitfield()).c_str());	//30 
			renderer->DrawTextInBox2D(connectionText, textBox, Vector2::TOP_LEFT, contentFontHeight, TEXT_DRAW_OVERRUN, RGBA::WHITE);
			textBox.Translate(newLine);
		}
	}

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

void NetSession::RegisterMessage(std::string name, NetSessionMessageCB messageCB)
{
	net_message_definition_t* messageDef = new net_message_definition_t();
	messageDef->m_messageCB = messageCB;
	messageDef->m_messageName = name;
	messageDef->m_messageID = (uint16_t) m_registeredMessages.size();
	m_registeredMessages.push_back(messageDef);
}

void NetSession::SortMessageIDs()
{
	std::sort(m_registeredMessages.begin(), m_registeredMessages.end(), CompareDefinitionsByName);
	for (int i = 0; i < (size_t) m_registeredMessages.size(); i++)
	{
		m_registeredMessages[i]->m_messageID = (uint16_t) i;
	}
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
		if (CheckRandomChance(m_simulatedLoss)){
			ConsolePrintf(RGBA::RED, "Packet lost.");
			return;
		}

		time_stamped_packet_t* timedPacket = new time_stamped_packet_t();

		timedPacket->m_packet->WriteBytes(read, buffer);
		timedPacket->m_fromConnection = GetConnectionByAddress(from_addr);
		timedPacket->AddSimulatedLatency(m_simulatedLatency);

		m_timeStampedPackets.push_back(timedPacket);

		std::sort(m_timeStampedPackets.begin(), m_timeStampedPackets.end(), TimeStampedPacketCompare);

		//NetPacket* packet = new NetPacket();

		//NetMessage message;
		//get the size of the entire buffer with the same method we used for RCS
		//packet->WriteBytes(read, buffer);		//writes the rest of the buffer to the NetMessage packer
		
		
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
		// check to see if the sender is actually somebody connected to us? seems relevant?
		//NetConnection* connection = GetConnectionByAddress(from);
		if (from != nullptr){
			net_message_definition_t* definition = GetRegisteredMessageByID(messageType);
			((definition->m_messageCB))( message, net_sender_t(from));
		}
	}
}

void NetSession::SendPacketsForConnection(unsigned int connectionIndex)
{
	NetPacket* packet = new NetPacket();
	packet->AdvanceWriteHead(sizeof(packet_header_t));
	uint8_t messagesSent = 0;
	for (int msgNum = 0; msgNum < m_connections[connectionIndex]->m_messagesToSend.size(); msgNum++){
		NetMessage* msg = m_connections[connectionIndex]->m_messagesToSend[msgNum];

		// write the message to the netpacket
		// if you dont' have the space to write a packet, send existing packet,
		// and start writing a new packet
		if (!packet->WriteMessage(*msg)){
			SendPacketToConnection(connectionIndex, packet, messagesSent);
			messagesSent = 0;
			delete packet;
			packet = new NetPacket();
			packet->WriteMessage(*msg);		//this is always guaranteed to be smaller than net packet size
		}
		messagesSent++;
		//m_socket->SendTo(connection->GetAddress(), msg->GetBuffer(), msg->GetWrittenByteCount());
	}
	if (messagesSent > 0){
		SendPacketToConnection(connectionIndex, packet, messagesSent);
	}

	m_connections[connectionIndex]->ClearMessageQueue();
}

void NetSession::ProcessPackets()
{
	for (size_t i = 0; i < m_timeStampedPackets.size(); i++){
		time_stamped_packet_t* tsPacket = m_timeStampedPackets[i];
		if (tsPacket->m_timeToProcess < GetCurrentTimeSeconds()){
			//process a packet
			packet_header_t packetHeader;
			tsPacket->m_packet->ReadHeader(packetHeader);
			for (unsigned int i = 0; i < packetHeader.m_unreliableMessageCount; i++){
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

void NetSession::SendPacketToConnection(uint8_t connIndex, NetPacket * packet, uint8_t numMessages)
{
	NetConnection* connToSendTo = m_connections[connIndex];
	packet_header_t packetHeader;
	packetHeader.m_unreliableMessageCount = numMessages;
	packetHeader.m_senderConnectionIndex = GetLocalConnectionIndex();
	packet->WriteHeader(packetHeader);
	if (m_socket->SendTo(connToSendTo->GetAddress(), packet->GetBuffer(), packet->GetWrittenByteCount()) == 0U){
		RemoveConnection(connIndex);
	}
}

bool TimeStampedPacketCompare(time_stamped_packet_t * leftHandSide, time_stamped_packet_t * rightHandSide)
{
	return leftHandSide->m_timeToProcess < rightHandSide->m_timeToProcess;
}

bool OnHeartbeat(NetMessage msg, net_sender_t const & from)
{
	//ConsolePrintf(RGBA::GREEN.GetColorWithAlpha(165), "Heartbeat from connection %s", from.m_connection->GetAddress().ToString().c_str()); 
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
