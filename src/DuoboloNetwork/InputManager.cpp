#include <DuoBoloNetwork/InputManager.h>
#include <DuoBoloNetwork/BinarySerialize.h>

#include <raylib.h>
#include <stdexcept>

InputManager* InputManager::s_instance = nullptr;

InputManager::InputManager()
{
	if (s_instance != nullptr)
		throw std::runtime_error("There is more than 1 input manager object");

	s_instance = this;
}

InputManager::~InputManager()
{
	s_instance = nullptr;
}

void InputManager::PollInputs()
{
	for(auto& input : m_keyInputMap)
	{
		if(IsKeyPressed(input.first))
		{
			InternalPerformKey(input.second, PressState::Press);
		}

		if(IsKeyReleased(input.first))
		{
			InternalPerformKey(input.second, PressState::Release);
		}

		if(IsKeyDown(input.first))
		{
			InternalPerformKey(input.second, PressState::Down);
		}
	}

	for (auto& mouse : m_mouseInputMap)
	{
		if(IsMouseButtonPressed(mouse.first))
		{
			InternalPerformMouse(mouse.second, PressState::Press);
		}

		if(IsMouseButtonReleased(mouse.first))
		{
			InternalPerformMouse(mouse.second, PressState::Release);
		}

		if(IsMouseButtonDown(mouse.first))
		{
			InternalPerformMouse(mouse.second, PressState::Down);
		}
	}
}

void InputManager::BindKey(KeyboardKey key, std::string action)
{
	if(m_keyInputMap.contains(key))
	{
		m_keyInputMap.erase(key);
	}

	m_keyInputMap[key] = std::move(action);
}

void InputManager::BindMouse(MouseButton mouse, std::string action)
{
	if(m_mouseInputMap.contains(mouse))
	{
		m_mouseInputMap.erase(mouse);
	}

	m_mouseInputMap[mouse] = std::move(action);
}

void InputManager::BindMouseAction(std::string action, std::function<void(PressState)> callback)
{
	m_mouseActionMap[action] = std::move(callback);
}

void InputManager::BindKeyboardAction(std::string action, std::function<void(PressState)> callback)
{
	m_keyActionMap[action] = std::move(callback);
}

InputManager& InputManager::Instance()
{
	return *s_instance;
}

void InputManager::InternalPerformKey(const std::string& action, PressState state)
{
	auto it = m_keyActionMap.find(action);
	if (it == m_keyActionMap.end())
		return;

	m_keyActionMap[action](state);
}

void InputManager::InternalPerformMouse(const std::string& action, PressState state)
{
	auto it = m_mouseActionMap.find(action);
	if (it == m_mouseActionMap.end())
		return;

	m_mouseActionMap[action](state);
}
