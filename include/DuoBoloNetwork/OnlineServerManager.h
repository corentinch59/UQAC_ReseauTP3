#pragma once

#include <DuoBoloNetwork/OnlineManager.h>

class OnlineServerManager final : OnlineManager
{
	public:
		explicit OnlineServerManager(unsigned short port, int numberOfPlayers);
		~OnlineServerManager() override;

		OnlineServerManager(const OnlineServerManager&) = delete;
		OnlineServerManager(OnlineServerManager&&) = delete;

		OnlineServerManager& operator=(const OnlineServerManager&) = delete;
		OnlineServerManager& operator=(OnlineServerManager&&) = delete;

		void PollEvents();

	private:
};

