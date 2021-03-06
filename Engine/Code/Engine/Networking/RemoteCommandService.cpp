#include "RemoteCommandService.hpp"
#include "Engine/Math/Renderer.hpp"
#include "Engine/Core/DevConsole.hpp"

#include "Game/EngineBuildPreferences.hpp"

RemoteCommandService* RemoteCommandService::g_remoteCommandService = nullptr;


RemoteCommandService::RemoteCommandService()
{
}

RemoteCommandService::~RemoteCommandService()
{
	delete m_listenSocket;
}

RemoteCommandService * RemoteCommandService::GetInstance()
{
	if (g_remoteCommandService == nullptr){
		g_remoteCommandService = new RemoteCommandService();
		if (!g_remoteCommandService->Startup()){
			LogErrorf("RemoteCommandService startup failed");
			delete g_remoteCommandService;
			g_remoteCommandService = nullptr;
		}
	}
	return g_remoteCommandService;
}

#if defined(RCS_ENABLED)
bool RemoteCommandService::Startup()
{
	m_isRunning = true;
	return true;
}

void RemoteCommandService::Shutdown()
{
	if (!m_listenSocket->IsClosed()){
		m_listenSocket->Close();
	}
	m_isRunning = false;
}

void RemoteCommandService::Update()
{
	switch (m_currentState){
		case RCS_STATE_INITIAL:
			UpdateInitial();
			break;
		case RCS_STATE_HOST:
			UpdateHost();
			break;
		case RCS_STATE_CLIENT:
			UpdateClient();
			break;
	}
}

void RemoteCommandService::Render(Renderer* r, AABB2 screenBounds)
{
	screenBounds.AddPaddingToSides(-.005f, -.005f);
	AABB2 textBox = screenBounds.GetPercentageBox(Vector2(.6f, .986f), Vector2(1.f,1.f));
	float height = textBox.GetHeight();
	float textHeight = height * .95f;
	//print header
	std::string headerString = "";
	switch (m_currentState){
	case RCS_STATE_INITIAL:
		headerString = "Attempting to connect/host...";
		break;
	case RCS_STATE_HOST:
		headerString = Stringf("%s [HOST]", m_listenSocket->m_address.ToString().c_str());
		break;
	case RCS_STATE_CLIENT:
		if (m_connections.size() > 0){
			headerString = Stringf("%s [CLIENT]", m_connections[0]->m_address.ToString().c_str());
		} else {
			headerString = "Lost Connection to Host...";
		}
		break;
	}
	r->DrawTextInBox2D(headerString, textBox, Vector2 (1.f, .5f), height, TEXT_DRAW_SHRINK_TO_FIT);
	textBox.Translate(Vector2(0.f, -height * 1.5f));
	
	r->DrawTextInBox2D(Stringf("Connections: %i", (int) m_connections.size()), textBox, Vector2 (1.f, .5f), height, TEXT_DRAW_SHRINK_TO_FIT);
	textBox.Translate(Vector2(0.f, -height));
	for(int i = 0; i < (int) m_connections.size(); i++){
		std::string connectionString = Stringf("[C%i] %s", i, m_connections[i]->m_address.ToString().c_str());
		r->DrawTextInBox2D(connectionString, textBox, Vector2 (1.f, .5f), textHeight, TEXT_DRAW_SHRINK_TO_FIT);
		textBox.Translate(Vector2(0.f, -height));
	}
}

bool RemoteCommandService::JoinRCS(std::string addr)
{
	DisconnectAll();
	NetAddress addrToJoin;
	if (addr == "" || addr == "local"){
		addrToJoin = NetAddress::GetLocal(REMOTE_COMMAND_SERVICE_PORT);
	} else {
		addrToJoin = NetAddress(addr);
	}
	TCPSocket* connectionSocket = new TCPSocket();
	if (connectionSocket->Connect(addrToJoin)){
		connectionSocket->SetUnblocking();
		//connectionSocket->Listen()
		m_connections.push_back(connectionSocket);
		m_packers.push_back(new BytePacker(BIG_ENDIAN));
		m_currentState = RCS_STATE_CLIENT;
		return true;
	} else {
		delete connectionSocket;
		m_currentState = RCS_STATE_INITIAL;
		return false;
	}
}

bool RemoteCommandService::HostRCS(std::string port)
{
	DisconnectAll();
	if (port == ""){
		port = REMOTE_COMMAND_SERVICE_PORT;
	}
	m_listenSocket = new TCPSocket();
	
	if (m_listenSocket->Listen(port, MAX_REMOTE_CONNECTIONS)){
		m_listenSocket->SetUnblocking();
		ConsolePrintf(RGBA::YELLOW, "Remote Command Service Started On Port %s", port.c_str());
		m_currentState = RCS_STATE_HOST;
		return true;
	} else {
		delete m_listenSocket;
		m_listenSocket = nullptr;
		m_currentState = RCS_STATE_INITIAL;
		return false;
	}
}

void RemoteCommandService::ProcessNewConnections()
{

	TCPSocket* newSocket = m_listenSocket->Accept();
	//found a new connections
	if (newSocket != nullptr){
		LogTaggedPrintf("net", "Added new connection: %s", newSocket->m_address.ToString().c_str());
		m_connections.push_back(newSocket);
		m_packers.push_back(new BytePacker(BIG_ENDIAN));
	}
	/*
	while (!m_listenSocket->IsClosed()) {
		TCPSocket *them = m_listenSocket->Accept();
		if (them != nullptr) {
			byte_t buffer[1 * KB]; 
			unsigned int received = them->Receive( buffer, (1 * KB) - 1U ); 
			if (received > 0U) {
				buffer[received] = NULL;
				ConsolePrintf(RGBA::YELLOW, "Receieved: %s", buffer ); 

				them->Send( "Thank", 6 ); 
			}

			delete them; 
		}
	}
	*/
}

void RemoteCommandService::ProcessAllConnections()
{
	for (int connectionIndex = 0; connectionIndex < (int) m_connections.size(); connectionIndex++){
		m_currentConnectionIndex = connectionIndex;
		ReceiveDataOnSocket(connectionIndex);
	}
}

void RemoteCommandService::ProcessMessage(TCPSocket * socket, BytePacker * payload)
{
	UNUSED(socket);
	bool isEcho = true;
	payload->ReadBytes(&isEcho, 1);

	char str[256];
	if (payload->ReadString(str, 256)){		//read & write are specialty functions - it does some work for you
		//// if everything is right, could guarantee that the readable byte count is 0 at this point
		//ASSERT( payload->GetReadableByteCount() == 0);

		// succeeded in getting a command/string
		if (isEcho) {
			//Print this to the console, can format how you want
			if (m_shouldEcho){
				ConsolePrintf(RGBA(84,84,128,255), "[C%i ECHO] %s", m_currentConnectionIndex, str );
			}
		} else {
			//Otherwise, it's a command, so ~do it ~
			//RunCommand(str);
			ConsolePrintf(RGBA(128,84,128,255), "[C%i COMMAND] %s", m_currentConnectionIndex, str );
			m_consoleReference->AddHook(RCSEchoHook);
			CommandRun(str);
			m_consoleReference->RemoveHook(RCSEchoHook);

		}
	}
}

void RemoteCommandService::CleanupDisconnects()
{
	for (int i = ( int) m_connections.size() - 1; i >= 0 ; i--){
		TCPSocket* connection = m_connections[i];
		if (connection->IsClosed()){
			RemoveAtFast(m_connections, i);
			delete connection;
		}
	}
}

void RemoteCommandService::DisconnectAll()
{
	if (m_listenSocket != nullptr){
		m_listenSocket->Close();
		delete m_listenSocket;
		m_listenSocket = nullptr;
	}
	for (int i = 0; (int) i < m_connections.size(); i++){
		m_connections[i]->Close();
	}
	CleanupDisconnects();
	m_currentState = RCS_STATE_INITIAL;
}

void RemoteCommandService::SendMessageAll(std::string msgString, bool isEcho)
{
	for (int i = 0; i < ( int) m_connections.size(); i++){
		SendAMessageToAHotSingleClientInYourArea(i, msgString, isEcho);
	}
	CommandRun(msgString.c_str());
}

void RemoteCommandService::SendMessageBroadcast(std::string msgString, bool isEcho)
{
	for (int i = 0; i < ( int) m_connections.size(); i++){
		SendAMessageToAHotSingleClientInYourArea(i, msgString, isEcho);
	}
}

void RemoteCommandService::SendAMessageToAHotSingleClientInYourArea(unsigned int connectionIndex, std::string msgString, bool isEcho)
{
	if ((int) connectionIndex > (int) m_connections.size()-1){
		return;
	}
	BytePacker message = BytePacker(BIG_ENDIAN); 
	TCPSocket *sock = m_connections[connectionIndex];
	if (sock == nullptr) {
		return; 
	}

	message.WriteBytes( 1, (void*) &isEcho );  
	message.WriteString(msgString.c_str());

	size_t len = message.GetWrittenByteCount() + 1; 
	if( len > 0xffff){
		ConsolePrintf(RGBA::RED, "bytepacker format doesn't support format > max ushort"); // format doesn't support lengths larger than a max ushort; 
	}
	uint16_t uintlen = (uint16_t)len; 
	ToEndianness(sizeof(uintlen), &uintlen , BIG_ENDIAN); 
	sock->Send( (void*) &uintlen , sizeof(uintlen));  // templated overload
	sock->Send(  message.GetBuffer(), len ); 

}

void RemoteCommandService::SendEchoMessage(std::string msgString)
{
	SendAMessageToAHotSingleClientInYourArea(m_currentConnectionIndex, msgString, true);
}

void RemoteCommandService::ReceiveDataOnSocket(int connectionIndex)
{
	ASSERT_OR_DIE(connectionIndex < (int) m_connections.size(), "Invalid connection index");
	TCPSocket *tcp = m_connections[connectionIndex];
	BytePacker *buffer = m_packers[connectionIndex];
	
	bool isReadyToProcess = false;
	do {
		isReadyToProcess = false;
		if (tcp->IsClosed()){
			ConsolePrintf(RGBA::RED, "Connection closed unexpectedly.");
			return;
		}


		//gotta read until it's two - get the size of the packet
		if (buffer->GetWrittenByteCount() < 2){
			//write size data into buffer
			size_t read = tcp->Receive (buffer->GetWriteHeadLocation(), 2 - buffer->GetWrittenByteCount());
			if (read == 0){
				return;
			}
			buffer->AdvanceWriteHead( read );
		}

		//once you have two, how many more bytes to get?
		if (buffer->GetWrittenByteCount() >= 2) {
			uint16_t len;
			byte_t* outLength = (byte_t*) malloc(sizeof(uint16_t));
			buffer->ReadBytes(outLength, sizeof(uint16_t));		//should be 2?
			FromEndianness(2, outLength, BIG_ENDIAN);
			len = (uint16_t)(outLength[0] + outLength[1]);
			free(outLength);

			// the packet has 2 bytes
			// len is everything that's in your "payload"
			//		(always send length, then payload)
			// Whatever application you're writing will deal with the payload, we're just putting it on the truck.
			// Basically, we're just reading piece by piece until we get len amount.
			// <Full size of the payload> + 2 - <what we've written so far> = remaining amount of the payload;
			// [messageLength (total)] [isEcho] [payload_message] ......... 
			unsigned int bytesNeeded = len + 2U - (unsigned int)buffer->GetWrittenByteCount();			//the packet has 2 bytes
			if (bytesNeeded > 0) {
				size_t read = tcp->Receive( buffer->GetWriteHeadLocation(), bytesNeeded);
				buffer->AdvanceWriteHead(read);
				//bytes_needed -= read;
			}

			isReadyToProcess = (bytesNeeded == 0);
			if (!isReadyToProcess){
				buffer->ResetRead();
			}
		}

	} while (!isReadyToProcess);

	//ToEndianness(buffer->GetWrittenByteCount(), buffer->GetBuffer(), BIG_ENDIAN);
	ProcessMessage( tcp, buffer);
	//gotta read frame-to-frame, can't block. Ready 2 receive, always
	buffer->ResetWrite();
	buffer->ResetRead();
}

void RemoteCommandService::SetShouldEcho(bool shouldEcho)
{
	m_shouldEcho = shouldEcho;
}



void RemoteCommandService::UpdateInitial()
{
	if (!JoinRCS()){
		if (!HostRCS()){
			//else: delay lol
		}
	}
	
}

void RemoteCommandService::UpdateClient()
{
	//ProcessAllConnections
	ProcessAllConnections();
	//CleanupDisconnects
	CleanupDisconnects();
	//if (no Host)
	//		Go Back To Initial
	if (m_connections.size() == 0){
		m_currentState = RCS_STATE_INITIAL;
	}

}

void RemoteCommandService::UpdateHost()
{
	//Process New Connections
	ProcessNewConnections();
	//Process All Connections
	ProcessAllConnections();
	//Cleanup Disconnects
	CleanupDisconnects();
}

#else
bool RemoteCommandService::Startup()
{
	return true;
}

void RemoteCommandService::Shutdown()
{

}

void RemoteCommandService::Update()
{

}

void RemoteCommandService::Render(Renderer*, AABB2)
{

}

bool RemoteCommandService::JoinRCS(std::string)
{
	return true;
}

bool RemoteCommandService::HostRCS(std::string )
{
	return true;
}

void RemoteCommandService::SetShouldEcho(bool)
{

}

void RemoteCommandService::SendMessageAll(std::string, bool)
{

}

void RemoteCommandService::SendMessageBroadcast(std::string, bool)
{
}

void RemoteCommandService::SendAMessageToAHotSingleClientInYourArea(unsigned int, std::string, bool)
{
}

void RemoteCommandService::ProcessNewConnections()
{
}

void RemoteCommandService::SendEchoMessage(std::string)
{

}

#endif

void RemoteCommandServiceListen()
{
	RemoteCommandService::GetInstance()->ProcessNewConnections();
}

void RemoteCommandServiceUpdate()
{
	while (RemoteCommandService::GetInstance()->m_isRunning){
		RemoteCommandService::GetInstance()->Update();
	}
}
