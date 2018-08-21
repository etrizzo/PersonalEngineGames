#include "Engine/Core/LogSystem.hpp"

struct sockaddr;

struct net_address_t
{
public:
	net_address_t(); 
	net_address_t( sockaddr const *addr ); 
	net_address_t( char const *string ); 

	bool to_sockaddr( sockaddr *out, size_t *out_addrlen ); 
	bool from_sockaddr( sockaddr const *sa ); 

	std::string to_string() const; 

public:
	unsigned int ip4_address;
	uint16_t port; 

public: // STATICS
	static net_address_t GetLocal(); 

	/* these are optional, but useful helpers
	static uint GetAllForHost( net_address_t *out, 
	uint max_count, 
	char const *host_name, 
	char const *service_name ); 
	static uint GetAllLocal( net_address_t *out, uint max_count ); */
};


class Net
{
public:
	static bool Startup();
	static bool Shutdown();

};

void LogIP( sockaddr_in* in);

bool GetAddressForHost( sockaddr * out, int * out_addrlen, char const* hostname, char const* service = "12345");
void GetAddressExample();


void ConnectExample( net_address_t const & addr, char const* msg);