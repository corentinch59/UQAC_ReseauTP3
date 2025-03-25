#include <DuoBoloNetwork/Timer.h>


Timer::Timer(float targetedTime, std::function<void()> endFunc, float delay, bool shouldLoop) :
Timer(targetedTime, endFunc, std::function<void(float, float)>(), delay, shouldLoop)
{
}

Timer::Timer(float targetedTime, std::function<void()> endFunc, std::function<void(float, float)> continuousFunc, float delay, bool shouldLoop) :
m_isPlaying(true),
m_isLooping(shouldLoop),
m_isFinished(false),
m_currentTime(0),
m_delay(delay),
m_targetTime(targetedTime),
m_timerEndFunc(endFunc),
m_continuousFunc(continuousFunc)
{
}

void Timer::Play()
{
	m_isPlaying = true;
}

void Timer::Pause()
{
	m_isPlaying = false;
}

void Timer::Reset(bool triggerFunc)
{
	m_currentTime = 0;
	if (triggerFunc && m_timerEndFunc)
		m_timerEndFunc();
}

void Timer::Stop(bool triggerFunc)
{
	m_isFinished = true;
	if (triggerFunc && m_timerEndFunc)
		m_timerEndFunc();
}

void Timer::CallContinuousFunc(float deltaTime)
{
	if (m_continuousFunc)
		m_continuousFunc(deltaTime, m_currentTime);
}

void Timer::Update(float deltaTime)
{
	if (m_isFinished || !m_isPlaying)
		return;
	
	if (m_delay > 0)
	{
		m_delay -= deltaTime;
		return;
	}
	else
	{
		m_currentTime += deltaTime;
	}

	if (m_currentTime < m_targetTime)
	{
		CallContinuousFunc(deltaTime);
		return;
	}

	if (m_isLooping)
	{
		Reset(true);
	}
	else
	{
		Stop(true);
	}
}
