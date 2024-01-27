#include "Core.hpp"

InputHandler::InputHandler() : m_KeyboardState(SDL_GetKeyboardState(NULL)) { }

bool InputHandler::GetKey(const SDL_Scancode scanCode) const {
	return m_KeyboardState[scanCode] == 1;
}

bool InputHandler::GetKeyUp(const SDL_Scancode scanCode) const {
	if (m_CurrentEvent.type != SDL_KEYUP || m_CurrentEvent.key.repeat == 1) {
		return false;
	}

	return m_CurrentEvent.key.keysym.sym == SDL_GetKeyFromScancode(scanCode);
}

bool InputHandler::GetKeyDown(const SDL_Scancode scanCode) const {
	if (m_CurrentEvent.type != SDL_KEYDOWN || m_CurrentEvent.key.repeat == 1) {
		return false;
	}

	return m_CurrentEvent.key.keysym.sym == SDL_GetKeyFromScancode(scanCode);
}

bool InputHandler::GetMouseButton(const uint8_t buttonCode) const {
	return false;
}

bool InputHandler::GetMouseButtonUp(const uint8_t buttonCode) const {
	if (m_CurrentEvent.type != SDL_MOUSEBUTTONUP) {
		return false;
	}

	return m_CurrentEvent.button.button == buttonCode;
}

bool InputHandler::GetMouseButtonDown(const uint8_t buttonCode) const {
	if (m_CurrentEvent.type != SDL_MOUSEBUTTONDOWN) {
		return false;
	}

	return m_CurrentEvent.button.button == buttonCode;
}

void InputHandler::PollEvents() {
	SDL_PollEvent(&m_CurrentEvent);
}