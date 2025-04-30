#pragma once

#include <DuoBoloShared/OnlineManager.h>

class OnlineServerManager final : public OnlineManager
{
	public:
		explicit OnlineServerManager(unsigned short port, int numberOfPlayers);
		~OnlineServerManager() override;

		OnlineServerManager(const OnlineServerManager&) = delete;
		OnlineServerManager(OnlineServerManager&&) = delete;

		OnlineServerManager& operator=(const OnlineServerManager&) = delete;
		OnlineServerManager& operator=(OnlineServerManager&&) = delete;

	private:
};

