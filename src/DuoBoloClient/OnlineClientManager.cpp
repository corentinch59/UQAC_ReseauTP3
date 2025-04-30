#include <DuoBoloClient/OnlineClientManager.h>
#include <enet6/enet.h>
#include <spdlog/spdlog.h>

OnlineClientManager::OnlineClientManager() :
mClient(nullptr)
{
}

OnlineClientManager::~OnlineClientManager()
{
}

bool OnlineClientManager::SendConnectionRequest(unsigned short port, const std::string& ipAddress)
{
	ENetAddress address;
	address.port = port;

	if(enet_address_set_host(&address, ENET_ADDRESS_TYPE_IPV4, ipAddress.data()) != 0)
	{
		spdlog::error("failed to resolve address");
		return false;
	}

	if(mHost)
		enet_host_destroy(mHost);

	mHost = enet_host_create(
		ENET_ADDRESS_TYPE_IPV4,
		nullptr,
		1,
		2,
		0,
		0
	);

	if(!mHost)
	{
		spdlog::error("Failed to create host");
		return false;
	}

	mClient = enet_host_connect(mHost, &address, 2, 0);
	if (mClient == nullptr)
	{
		spdlog::error("Failed to connect to server : {}", ipAddress);
		return false;
	}

	ENetEvent event;
	if (enet_host_service(mHost, &event, 1000.f) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		spdlog::info("Client successfully connected to {}", ipAddress);
	}
	else
	{
		spdlog::error("connection failed {}", (int) event.type);
		return false;
	}

	return true;
}

void OnlineClientManager::SendPacket(ENetPacket* packet, std::uint8_t channel)
{
	if (mClient && packet)
	{
		spdlog::info("Sending packet of {} bytes", packet->dataLength);
		enet_peer_send(mClient, channel, packet);
	}
}

void OnlineClientManager::Disconnect()
{
	enet_peer_disconnect(mClient, 0);
}
