#pragma once
#include "Engine/Networking/UDPSocket.hpp"
#include "Engine/Networking/NetMessage.hpp"

class NetPacket;
class NetConnection;
class Renderer;
#define MAX_CONNECTIONS (16)
#define DEFAULT_HEARTBEAT (.33f)
#define DEFAULT_SESSION_SEND_RATE_HZ (50.f)		//NOTE: Hz, not ms.

//net session UI settings
#define SESSION_FONT_HEADER_SIZE (.015f)
#define SESSION_FONT_CONTENT_SIZE (.008f)
#define SESSION_LINE_MULTIPLIER (1.04f)

enum eNetCoreMessages : uint8_t{
	NETMSG_PING = 0,			// unreliable, connectionless
	NETMSG_PONG, 				// unreliable, connectionless
	NETMSG_HEARTBEAT,			// unreliable

	NETMSG_UNRELIABLE_TEST = 128,
	NETMSG_RELIABLE_TEST = 129, // reliable


	NETMSG_CORE_COUNT,
};


struct time_stamped_packet_t{
public:
	time_stamped_packet_t();
	~time_stamped_packet_t();

	void AddSimulatedLatency(IntRange simulatedLatencySeconds);
	bool IsValidConnection() const;

	double m_timeToProcess;
	NetPacket* m_packet;
	NetConnection* m_fromConnection;
};



//Game has a global session...
class NetSession
{
public:
	NetSession();
	void Startup();

	void Update();
	void RenderInfo(AABB2 screenBounds, Renderer* renderer);

	void SendDirect(NetMessage* message, NetConnection* invalidConnection);

	void SetHeartbeat(float hz);
	void SetSessionSendRate(float hz);
	void SetConnectionSendRate(int connectionIndex, float hz);

	// This creates the socket(s) we can communicate on; 
	void AddBinding(const char* port);

	NetConnection* AddConnection(uint8_t idx, NetAddress addr);
	void RemoveConnection(uint8_t idx);

	NetConnection* GetConnection(uint8_t idx) const;
	NetConnection* GetConnectionByAddress(NetAddress addr) const;
	NetConnection* GetLocalConnection() const;

	unsigned int GetLocalConnectionIndex() const;
	float GetSessionSendRateHZ() const;

	// Causes session to discard a certain ratio of incoming packets.
	// loss_amount is a value between 0 and 1
	// 0 means no loss
	// 1 means all loss.
	void SetSimulatedLoss( float lossAmount ); 

	// Adds an arbitrary wait time between when we receive a packet and when we process it
	// When receiving a packet, pick a random value between min and max
	// - max should be set to the Max( min, max ); Hence a default value of zero will
	//   actually result in the range being [min-min]. 
	void SetSimulatedLatency(  int minAddedLatencyMS = 0, int maxAddedLatencyMS = 0 ); 

	//adds the callback to the map of possible messages this session can handle
	void RegisterMessage(int msgIndex, std::string name, NetSessionMessageCB messageCB, eNetMessageOptions messageOptions);
	void SortMessageIDs();
	bool IsMessageRegistered(std::string name);
	bool IsMessageRegistered(uint16_t id);
	net_message_definition_t* GetRegisteredMessageByName(std::string name);
	net_message_definition_t* GetRegisteredMessageByID(uint8_t id);

	//Update loop
	void UpdateConnections();
	void ProcessIncoming();
	void ProcessOutgoing();

	void ProcessMessage(NetMessage message, NetConnection* from);

	//NetPacket* AddTrackedPacket(const packet_header_t& header);

	


protected:
	void SendPacketsForConnection(unsigned int connectionIndex);
	void ProcessPackets();
	void SendPacketToConnection(uint8_t connIndex, NetPacket* packet, uint8_t numMessages);

	
	NetConnection* m_connections[MAX_CONNECTIONS] = {nullptr};
	std::vector<net_message_definition_t*> m_registeredMessages;
	std::vector<net_message_definition_t*> m_unassignedMessages;
	std::vector<net_message_definition_t*> m_fixedIndexMessages;
	UDPSocket* m_socket;		// your node in the graph? 

	std::vector<time_stamped_packet_t*> m_timeStampedPackets = std::vector<time_stamped_packet_t*>();
	float m_sessionRateHz	= DEFAULT_SESSION_SEND_RATE_HZ;
	float m_simulatedLoss	= 0.f;
	IntRange m_simulatedLatency = IntRange(0);
	float m_sessionHeartbeat = DEFAULT_HEARTBEAT;


};

bool TimeStampedPacketCompare(time_stamped_packet_t* leftHandSide, time_stamped_packet_t* rightHandSide);


bool OnHeartbeat( NetMessage msg, net_sender_t const &from );
bool OnPing( NetMessage msg, net_sender_t const &from ) ;
bool OnPong( NetMessage msg, net_sender_t const & from) ;
bool OnNetMsgUnreliableTest( NetMessage msg, net_sender_t const& from);
bool OnNetMsgReliableTest( NetMessage msg, net_sender_t const& from) ;