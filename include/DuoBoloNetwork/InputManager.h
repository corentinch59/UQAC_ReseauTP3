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

class InputManager
{
	public:
		InputManager();
		~InputManager();

		InputManager(const InputManager&) = delete;
		InputManager(InputManager&&) = delete;

		InputManager& operator=(const InputManager&) = delete;
		InputManager& operator=(InputManager&&) = delete;

		void PollInputs();

		void BindKey(KeyboardKey key, std::string action);
		void BindMouse(MouseButton mouse, std::string action);
		void BindMouseAction(std::string action, std::function<void(PressState)> callback);
		void BindKeyboardAction(std::string action, std::function<void(PressState)> callback);

		static InputManager& Instance();

	private:
		static InputManager* s_instance;

		std::unordered_map<KeyboardKey, std::string> m_keyInputMap;
		std::unordered_map<std::string, std::function<void(PressState)>> m_keyActionMap;

		std::unordered_map<MouseButton, std::string> m_mouseInputMap;
		std::unordered_map<std::string, std::function<void(PressState)>> m_mouseActionMap;

		void InternalPerformKey(const std::string& action, PressState state);
		void InternalPerformMouse(const std::string& action, PressState state);
};

