#pragma once

#include <functional>
#include <string>

#include <raylib.h>

enum class PressState
{
	Press, // Once
	Release, //Once
	Down // Repeat
};

struct InputAction
{
	PressState state;
	float deltaTime;
};

class InputManager
{
	public:
		InputManager();
		~InputManager();

		InputManager(const InputManager&) = delete;
		InputManager(InputManager&&) = delete;

		InputManager& operator=(const InputManager&) = delete;
		InputManager& operator=(InputManager&&) = delete;

		void PollInputs(float dt);

		void BindKey(KeyboardKey key, std::string action);
		void BindMouse(MouseButton mouse, std::string action);
		void BindAction(std::string action, std::function<void(InputAction)> callback);

		static InputManager& Instance();

	private:
		static InputManager* s_instance;

		std::unordered_map<KeyboardKey, std::string> m_keyInputMap;
		std::unordered_map<MouseButton, std::string> m_mouseInputMap;

		std::unordered_map<std::string, std::function<void(InputAction)>> m_keyActionMap;

		void InternalPerformAction(const std::string& action, InputAction inputAction);
};

