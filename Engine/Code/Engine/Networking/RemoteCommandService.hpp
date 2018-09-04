#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Networking/TCPSocket.hpp"


#define REMOTE_COMMAND_SERVICE_PORT (29283)
#define MAX_REMOTE_CONNECTIONS (16)

class RemoteCommandService{
public:
	RemoteCommandService();
	~RemoteCommandService();

	void Startup();
	void Shutdown();



	std::vector<TCPSocket*> m_connections;
	TCPSocket* m_host;
	



	static RemoteCommandService* GetInstance();
	static RemoteCommandService* g_remoteCommandService;




};