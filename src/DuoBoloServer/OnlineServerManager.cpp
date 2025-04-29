#include <DuoBoloNetwork/OnlineServerManager.h>
#include <DuoBoloNetwork/PacketBuilder.h>
#include <spdlog/spdlog.h>

OnlineServerManager::OnlineServerManager(unsigned short port, int numberOfPlayers)
{
	ENetAddress address;
	enet_address_build_any(&address, ENET_ADDRESS_TYPE_IPV6);
	address.port = port;

	mHost = enet_host_create(
		ENET_ADDRESS_TYPE_ANY,
		&address,
		numberOfPlayers,	// Nb of clients
		2,					// Nb of channels
		0,					// incoming bandwidth limit
		0					// outgoing bandwidth limit
		);
}

OnlineServerManager::~OnlineServerManager()
{
	enet_host_destroy(mHost);
}

