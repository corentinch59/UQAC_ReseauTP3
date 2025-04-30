#pragma once

#include <string>
#include <cstdint>
#include <DuoBoloShared/OnlineManager.h>
#include <enet6/enet.h>

class OnlineClientManager final : public OnlineManager
{
	public:
		explicit OnlineClientManager();
		~OnlineClientManager() override;

		OnlineClientManager(const OnlineClientManager&) = delete;
		OnlineClientManager(OnlineClientManager&&) = delete;

		OnlineClientManager& operator=(const OnlineClientManager&) = delete;
		OnlineClientManager& operator=(OnlineClientManager&&) = delete;

		bool SendConnectionRequest(unsigned short port, const std::string& ipAddress);
		void SendPacket(ENetPacket* packet, std::uint8_t channel = 0);

		void Disconnect();

	private:
		ENetPeer* mClient;
};

