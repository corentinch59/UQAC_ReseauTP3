#include <DuoBoloShared/OnlineManager.h>

#include <spdlog/spdlog.h>

OnlineManager::OnlineManager() :
mHost(nullptr),
mListener(nullptr)
{
	spdlog::info("Initializing enet6...");
	if (enet_initialize() == 0)
		spdlog::info("Initialization successful");
}

OnlineManager::~OnlineManager()
{
	if (mHost)
		enet_host_destroy(mHost);
	enet_deinitialize();
}

void OnlineManager::PollEvents()
{
	ENetEvent event;
	while (enet_host_service(mHost, &event, 10) > 0)
	{
		do
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					mListener->OnConnected(event.peer);
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					mListener->OnDisconnected(event.peer);
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
				{
					mListener->OnTimedOut(event.peer);
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE:
				{
					std::vector<uint8_t> byteArray(event.packet->dataLength);
					std::memcpy(byteArray.data(), event.packet->data, event.packet->dataLength);
					mListener->OnPacketReceived(event.peer, mHost, byteArray);

					enet_packet_dispose(event.packet);
					break;
				}
			}
		} while (enet_host_check_events(mHost, &event) > 0);
	}
}


