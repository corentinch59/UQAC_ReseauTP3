#pragma once

#include <enet6/enet.h>

class OnlineManager
{
	public:
		OnlineManager();
		virtual ~OnlineManager();

		OnlineManager(const OnlineManager&) = delete;
		OnlineManager(OnlineManager&&) = delete;

		OnlineManager& operator=(const OnlineManager&) = delete;
		OnlineManager& operator=(OnlineManager&&) = delete;

	protected:
		ENetHost* mHost;
};

