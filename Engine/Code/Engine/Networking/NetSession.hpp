#pragma once
#include "Engine/Networking/UDPSocket.hpp"
#include "Engine/Networking/NetMessage.hpp"

class NetPacket;
class NetConnection;
class Renderer;
#define MAX_CONNECTIONS (16)
#define DEFAULT_HEARTBEAT (2.f)
#define DEFAULT_SESSION_SEND_RATE_HZ (30.f)		//NOTE: Hz, not ms.

#define MAX_NET_TIME_DILATION (0.1f)

#define JOIN_RESEND_TIME (.1f)
#define JOIN_TIMEOUT_TIME (10.f)

//net session UI settings
#define SESSION_FONT_HEADER_SIZE (.015f)
#define SESSION_FONT_CONTENT_SIZE (.0074f)
#define SESSION_LINE_MULTIPLIER (1.04f)

#define RELIABLE_WINDOW (64)

enum eNetCoreMessages : uint8_t{
	NETMSG_PING = 0,			// unreliable, connectionless
	NETMSG_PONG, 				// unreliable, connectionless
	NETMSG_HEARTBEAT,			// unreliable

	NETMSG_JOIN_REQUEST,		//unreliable, requires connection
	NETMSG_JOIN_DENY,			//unreliable, requires connection
	NETMSG_JOIN_ACCEPT,			//reliable in-order
	NETMSG_NEW_CONNECTION,		//reliable in-order
	NETMSG_JOIN_FINISHED,		//reliable in-order
	NETMSG_UPDATE_CONN_STATE,	//reliable in-order

	NETMSG_HANGUP,				//unreliable, requires connection

	NETMSG_UNRELIABLE_TEST,
	NETMSG_RELIABLE_TEST, // reliable
	NETMSG_SEQUENCE_TEST,


	NETMSG_CORE_COUNT,
};

enum eSessionState
{
	SESSION_DISCONNECTED = 0,  // Session can be modified     
	SESSION_BOUND,             // Bound to a socket - can send and receive connectionless messages.  No connections exist
	SESSION_CONNECTING,        // Attempting to connecting - waiting for response from the host
	SESSION_JOINING,           // Has established a connection, waiting final setup information/join completion
	SESSION_READY,             // We are fully in the session
};

enum eSessionError
{
	SESSION_OK,
	SESSION_ERROR_USER,                 // user disconnected
	SESSION_ERROR_INTERNAL,             // socket error; 

	SESSION_ERROR_JOIN_DENIED,          // generic deny error (release)
	SESSION_ERROR_JOIN_DENIED_NOT_HOST, // debug - tried to join someone who isn't hosting
	SESSION_ERROR_JOIN_DENIED_CLOSED,   // debug - not in a listen state
	SESSION_ERROR_JOIN_DENIED_FULL,     // debug - session was full 
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


	//Host Join
	void Host( char const *my_id, uint16_t port, uint16_t port_range = PORT_RANGE ); 
	void Join( char const *my_id, net_connection_info_t const &host_info ); 
	void Disconnect(); 

	void SetError( eSessionError error, char const *str = nullptr ); 
	void ClearError(); 
	eSessionError GetLastError( std::string *out_str = nullptr ); // get last error has an implicit clear in it


	//Old
	void Update();
	void UpdateSession();		//updates the session state

public: // should be private, but for ease we'll keep public
		// connection management
	NetConnection* CreateConnection( net_connection_info_t const &info ); 
	void DestroyConnection( NetConnection *cp );
	void BindConnection( uint8_t idx, NetConnection *cp ); 



	void RenderInfo(AABB2 screenBounds, Renderer* renderer);

	void SendDirect(NetMessage* message, NetConnection* invalidConnection);

	void SetHeartbeat(float hz);
	void SetSessionSendRate(float hz);
	void SetConnectionSendRate(int connectionIndex, float hz);

private:		//DEPRECATED for host-join
	// This creates the socket(s) we can communicate on; 
	bool AddBinding(const char* port, NetAddress& out_addr);
	//NetConnection* AddConnection(uint8_t idx, NetAddress addr);
	//void RemoveConnection(uint8_t idx);

	void UpdateConnecting();
	void UpdateJoining();

	StopWatch m_joinRequestTimer;
	StopWatch m_joinTimeoutTimer;


public:

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
	void RegisterMessage(int msgIndex, std::string name, NetSessionMessageCB messageCB, eNetMessageOptions messageOptions, uint8_t msgChannelIndex = (uint8_t) 0);
	void SortMessageIDs();
	bool IsMessageRegistered(std::string name);
	bool IsMessageRegistered(uint16_t id);
	net_message_definition_t* GetRegisteredMessageByName(std::string name);
	net_message_definition_t* GetRegisteredMessageByID(uint8_t id);

	//Update loop
	void UpdateConnections();
	void ProcessIncoming();
	void ProcessOutgoing();
	void DestroyDisconnects();

	void ProcessMessage(NetMessage* message, NetConnection* from);

	//NetPacket* AddTrackedPacket(const packet_header_t& header);

	
	// state management
	eSessionState m_state	= SESSION_DISCONNECTED; 

	// error handling
	eSessionError m_errorCode; 
	std::string m_errorString; 

	// Changes to how we store connections;
	std::vector<NetConnection*> m_allConnections;		//forseth uses a linked list??
	std::vector<NetConnection*> m_boundConnections;  // aka: m_connections_by_index

	NetConnection *m_myConnection                  = nullptr;     // convenience pointer
	NetConnection *m_hostConnection                = nullptr;   // convenience pointer; 

	unsigned int m_lastReceivedHostTimeMS;	// this is the time we received from the host + (RTT / 2)
	unsigned int m_desiredClientTimeMS;		// this is the time we want the client to eventually be
	unsigned int m_currentClientTimeMS;		// what the client actually reports return 

protected:
	void SendPacketsForConnection(unsigned int connectionIndex);
	void ProcessPackets();
	void SendPacketToConnection(uint8_t connIndex, NetPacket* packet, uint8_t numMessages, std::vector<NetMessage*> reliablesInPacket);

	
	//NetConnection* m_connections[MAX_CONNECTIONS] = {nullptr};
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

bool OnJoinRequest( NetMessage msg, net_sender_t const & from) ;
bool OnJoinDeny( NetMessage msg, net_sender_t const & from) ;
bool OnJoinAccept( NetMessage msg, net_sender_t const & from) ;
bool OnNewConnection( NetMessage msg, net_sender_t const & from) ;
bool OnJoinFinished( NetMessage msg, net_sender_t const & from) ;
bool OnUpdateConnectionState( NetMessage msg, net_sender_t const & from) ;

bool OnHangup(NetMessage msg, net_sender_t const & from);

bool OnNetMsgUnreliableTest( NetMessage msg, net_sender_t const& from);
bool OnNetMsgReliableTest( NetMessage msg, net_sender_t const& from) ;
bool OnNetMsgSequenceTest( NetMessage msg, net_sender_t const& from) ;