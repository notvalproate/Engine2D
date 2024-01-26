#include "Core.hpp"

InputHandler::InputHandler() : m_KeyboardState(SDL_GetKeyboardState(NULL)) { }

bool InputHandler::GetKeyDown(const SDL_Scancode scanCode) const {
	return m_KeyboardState[scanCode] == 1;
}

bool InputHandler::GetKeyUp(const SDL_Scancode scanCode) const {
	return m_KeyboardState[scanCode] == 0;
}

void InputHandler::PollEvents() {
	SDL_PollEvent(&m_CurrentEvent);
}