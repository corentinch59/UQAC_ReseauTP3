#pragma once

#include <memory>
#include <functional>

class Timer;

class TimerManager
{
public :
	TimerManager();
	~TimerManager();
	TimerManager(const TimerManager&) = delete;
	TimerManager(const TimerManager&&) noexcept = delete;

	TimerManager& operator=(const TimerManager&) = delete;
	TimerManager& operator=(const TimerManager&&) noexcept = delete;

	static void UpdateTimers(float deltaTime);
	static std::weak_ptr<Timer> CreateTimer(float timeToWait, const std::function<void()>& inCompletedAction, float inDelay = 0.f, bool isLooping = false);
	static std::weak_ptr<Timer> CreateContinuousTimer(float timeToWait, const std::function<void(float,float)>& inContinuousAction, const std::function<void()>& inCompletedAction = std::function<void()>(), float inDelay = 0.f, bool isLooping = false);
	static TimerManager& Instance();

private:
	static TimerManager* s_instance;
	std::vector<std::shared_ptr<Timer>> m_timerList;

};
