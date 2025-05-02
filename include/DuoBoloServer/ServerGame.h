#pragma once



#include <DuoBoloNetwork/Timer.h>
#include <cstdint>
#include <memory>

class ServerGame
{
public:
	ServerGame();
	~ServerGame();

	ServerGame(const ServerGame&) = delete;
	ServerGame(const ServerGame&&) noexcept = delete;

	ServerGame& operator=(const ServerGame&) = delete;
	ServerGame& operator=(const ServerGame&&) noexcept = delete;

	bool CanPlayerShoot(std::uint32_t id);

	static ServerGame& Instance();

private:
	static ServerGame* s_instance;
	std::unordered_map<std::uint32_t, std::weak_ptr<Timer>> mCDtimers;
};

