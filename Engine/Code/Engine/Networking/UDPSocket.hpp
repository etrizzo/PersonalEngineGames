#pragma once
#include "Engine/Networking/Socket.hpp"

// net/udpsocket.hpp

// when searching for addresses - you are no longer looking for AF_INET

//ETHERNET_MTU is the largest amount of data we can send.
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it; 
#define PACKET_MTU (ETHERNET_MTU - 40 - 8) 


// class test
#define GAME_PORT "10084"
#define PORT_RANGE 16


/*
A connection info is everything I need to know to create a connection for someone.  For now, this is just the following; 
*/
#define MAX_CONN_ID_LENGTH 16

struct net_connection_info_t
{
	NetAddress m_address; 
	/*
	`id` in this class will usually just be a unique user name.  
	In practice this is usually some GUID assigned to the player from the platform (SteamID, LiveID, PSN ID, etc...).  
	It is used to tell if someone is already in the session.
	*/
	char m_id[MAX_CONN_ID_LENGTH];
	uint8_t m_sessionIndex; 
};



class UDPSocket : public Socket
{
public: 
	//FORSETH QUESTION: How can the NetAddress be const if we want to do port range? would need to change it, right??
	bool Bind( NetAddress &addr, // address I want to listen for traffic on
		uint16_t port_range = 0U );		  // how many additional ports to bind on (so you bind from [addr.port,addr.port + port_range])

										  // return 0 on failure, otherwise how large is the next datagram
										  // TODO in class
	size_t SendTo( NetAddress const &addr, void const *data, size_t const byte_count );	
	size_t ReceiveFrom( NetAddress *out_addr, void *buffer, size_t const max_read_size );
	
	bool HasFatalError();

	
};

