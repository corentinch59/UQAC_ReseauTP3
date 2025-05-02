#include <DuoBoloServer/ServerGame.h>
#include <DuoBoloNetwork/TimerManager.h>
#include <stdexcept>

ServerGame* ServerGame::s_instance = nullptr;
constexpr float ShootCD = 0.1f;

ServerGame::ServerGame()
{
	if (s_instance != nullptr)
		throw std::runtime_error("There is more than 1 Timer manager object");

	s_instance = this;
}

ServerGame::~ServerGame()
{
	s_instance = nullptr;
}

bool ServerGame::CanPlayerShoot(std::uint32_t id)
{
	auto& timerWeak = mCDtimers[id];
	if (timerWeak.expired())
	{
		timerWeak = TimerManager::CreateTimer(ShootCD, []() {});
		return true;
	}
	return false;
}
