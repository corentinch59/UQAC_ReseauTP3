#include <DuoBoloNetwork/OnlineServerManager.h>
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

void OnlineServerManager::PollEvents()
{
	ENetEvent event;
	while(enet_host_service(mHost, &event, 10) > 0)
	{
		do
		{
			switch(event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					{
						ENetPeer* peer = event.peer;
						spdlog::info("new client connected : {}", peer->incomingPeerID);
						break;
					}
				case ENET_EVENT_TYPE_DISCONNECT:
					{
						ENetPeer* peer = event.peer;
						spdlog::info("client {} disconnected", peer->incomingPeerID);
						break;
					}
				case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
					{
						ENetPeer* peer = event.peer;
						spdlog::info("client {} timed out", peer->incomingPeerID);
						break;
					}
				case ENET_EVENT_TYPE_RECEIVE:
					{
						ENetPeer* peer = event.peer;
						spdlog::info("client {} sent data", peer->incomingPeerID);
						break;
					}
			}
		}
		while (enet_host_check_events(mHost, &event) > 0);
	}
}
