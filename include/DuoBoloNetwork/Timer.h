
#pragma once

#include <functional>


class Timer
{
	friend class TimerManager;

public:
	explicit Timer(float targetedTime, std::function<void()> endFunc, float delay, bool shouldLoop = false);
	explicit Timer(float targetedTime, std::function<void()> endFunc, std::function<void(float, float)> continuousFunc, float delay, bool shouldLoop = false);

	void Play();
	void Pause();
	void Reset(bool triggerFunc);
	void Stop(bool triggerFunc);

private:
	void CallContinuousFunc(float deltaTime);
	void Update(float deltaTime);

	bool m_isPlaying;
	bool m_isLooping;
	bool m_isFinished;

	float m_currentTime;
	float m_targetTime;
	float m_delay;
	std::function<void()> m_timerEndFunc;
	std::function<void(float, float)> m_continuousFunc;
};
