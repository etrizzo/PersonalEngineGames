#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Networking/TCPSocket.hpp"



#define REMOTE_COMMAND_SERVICE_PORT ("29283")
#define MAX_REMOTE_CONNECTIONS (16)

enum eRCSState{
	RCS_STATE_INITIAL,
	RCS_STATE_CLIENT,
	RCS_STATE_HOST,
	NUM_RCS_STATES
};

class RemoteCommandService{
public:
	RemoteCommandService();
	~RemoteCommandService();

	bool Startup();
	void Shutdown();

	void Update();
	void Render(Renderer* r, AABB2 screenBounds);

	bool JoinRCS();
	bool HostRCS();

	void ProcessNewConnections();
	//checks if you received any data
	void ProcessAllConnections();
	void ProcessMessage(TCPSocket* socket, BytePacker* payload);

	void SendMessageAll(std::string msgString);
	void SendAMessageToAHotSingleClientInYourArea(unsigned int connectionIndex, std::string msgString);

	void ReceiveDataOnSocket(int connectionIndex);

	std::vector<BytePacker*> m_packers;
	std::vector<TCPSocket*> m_connections;
	TCPSocket* m_listenSocket;
	eRCSState m_currentState = RCS_STATE_INITIAL;

	bool m_isRunning = true;
	



	static RemoteCommandService* GetInstance();
	static RemoteCommandService* g_remoteCommandService;

protected:
	void UpdateInitial();
	void UpdateClient();
	void UpdateHost();


};



void RemoteCommandServiceListen();
void RemoteCommandServiceUpdate();

