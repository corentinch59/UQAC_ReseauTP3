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

void InputManager::PollInputs(float dt)
{
	for(auto& input : m_keyInputMap)
	{
		if(IsKeyPressed(input.first))
		{
			InputAction ia{ PressState::Press, dt };
			InternalPerformAction(input.second, ia);
		}

		if(IsKeyReleased(input.first))
		{
			InputAction ia{ PressState::Release, dt };
			InternalPerformAction(input.second, ia);
		}

		if(IsKeyDown(input.first))
		{
			InputAction ia{ PressState::Down, dt };
			InternalPerformAction(input.second, ia);
		}
	}

	for (auto& mouse : m_mouseInputMap)
	{
		if(IsMouseButtonPressed(mouse.first))
		{
			InputAction ia{ PressState::Press, dt };
			InternalPerformAction(mouse.second, ia);
		}

		if(IsMouseButtonReleased(mouse.first))
		{
			InputAction ia{ PressState::Release, dt };
			InternalPerformAction(mouse.second, ia);
		}

		if(IsMouseButtonDown(mouse.first))
		{
			InputAction ia{ PressState::Down, dt };
			InternalPerformAction(mouse.second, ia);
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

void InputManager::BindKeyboardAction(std::string action, std::function<void(InputAction)> callback)
{
	m_keyActionMap[action] = std::move(callback);
}

InputManager& InputManager::Instance()
{
	return *s_instance;
}

void InputManager::InternalPerformAction(const std::string& action, InputAction inputAction)
{
	auto it = m_keyActionMap.find(action);
	if (it == m_keyActionMap.end())
		return;

	m_keyActionMap[action](inputAction);
}
