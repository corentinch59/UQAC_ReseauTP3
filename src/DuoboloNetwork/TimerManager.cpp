#include <DuoBoloNetwork/TimerManager.h>
#include <DuoBoloNetwork/Timer.h>
#include <stdexcept>

TimerManager* TimerManager::s_instance = nullptr;

TimerManager::TimerManager()
{
	if (s_instance != nullptr)
		throw std::runtime_error("There is more than 1 Timer manager object");

	s_instance = this;
}

TimerManager::~TimerManager()
{
	s_instance = nullptr;
}

void TimerManager::UpdateTimers(float deltaTime)
{
	const std::vector<std::shared_ptr<Timer>> timersToUpdate(TimerManager::Instance().m_timerList);
	std::vector<Timer*> timersToKill;

	for (auto it = timersToUpdate.begin(); it != timersToUpdate.end(); ++it)
	{
		it->get()->Update(deltaTime);
		if (it->get()->m_isFinished)
		{
			timersToKill.emplace_back(it->get());
		}
	}

	for (size_t i = 0; i < timersToKill.size(); ++i)
	{
		for (auto it = TimerManager::Instance().m_timerList.begin(); it != TimerManager::Instance().m_timerList.end();)
		{
			if (it->get() == timersToKill[i])
			{
				it = TimerManager::Instance().m_timerList.erase(it);
				break;
			}
			++it;
		}
	}
}

std::weak_ptr<Timer> TimerManager::CreateTimer(float timeToWait, const std::function<void()>& inCompletedAction, float inDelay, bool isLooping)
{
	std::shared_ptr<Timer> ret = std::make_shared<Timer>(timeToWait, inCompletedAction, inDelay, isLooping);
	TimerManager::Instance().m_timerList.push_back(ret);
	return ret;
}

std::weak_ptr<Timer> TimerManager::CreateContinuousTimer(float timeToWait, const std::function<void(float, float)>& inContinuousAction, const std::function<void()>& inCompletedAction, float inDelay, bool isLooping)
{
	std::shared_ptr<Timer> ret = std::make_shared<Timer>(timeToWait, inCompletedAction, inContinuousAction, inDelay, isLooping);
	TimerManager::Instance().m_timerList.push_back(ret);
	return ret;
}

TimerManager& TimerManager::Instance()
{
	return *s_instance;
}
