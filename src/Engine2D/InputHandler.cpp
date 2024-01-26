#include "Core.hpp"

InputHandler::InputHandler() : m_KeyboardState(SDL_GetKeyboardState(NULL)) { }

bool InputHandler::GetKey(const SDL_Scancode scanCode) const {
	return m_KeyboardState[scanCode] == 1;
}

bool InputHandler::GetKeyDown(const SDL_Scancode scanCode) const {
	if (m_CurrentEvent.type != SDL_KEYDOWN || m_CurrentEvent.key.repeat == 1) {
		return false;
	}

	return m_CurrentEvent.key.keysym.sym == SDL_GetKeyFromScancode(scanCode);
}

bool InputHandler::GetKeyUp(const SDL_Scancode scanCode) const {
	if (m_CurrentEvent.type != SDL_KEYUP || m_CurrentEvent.key.repeat == 1) {
		return false;
	}

	return m_CurrentEvent.key.keysym.sym == SDL_GetKeyFromScancode(scanCode);
}

void InputHandler::PollEvents() {
	SDL_PollEvent(&m_CurrentEvent);
}