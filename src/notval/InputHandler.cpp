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
	return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(buttonCode);
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
	SetProperties();
}

void InputHandler::SetProperties() {
	if (m_CurrentEvent.type == SDL_MOUSEMOTION) {
		SDL_GetMouseState(&mousePositionX, &mousePositionY);
	}
	else if (
		m_CurrentEvent.type == SDL_KEYDOWN && 
		(	
			m_CurrentEvent.key.keysym.sym == SDLK_BACKSPACE || 
			m_CurrentEvent.key.keysym.sym == SDLK_RETURN || 
			m_CurrentEvent.key.keysym.sym == SDLK_TAB || 
			m_CurrentEvent.key.keysym.sym == SDLK_DELETE
		)
	) 
	{
		inputString = m_CurrentEvent.key.keysym.sym;
	}
	else if (m_CurrentEvent.type == SDL_TEXTINPUT) {
		inputString = *m_CurrentEvent.text.text;
	}
	else if (m_CurrentEvent.type == SDL_MOUSEWHEEL) {
		mouseScrollDeltaX = m_CurrentEvent.wheel.x;
		mouseScrollDeltaY = m_CurrentEvent.wheel.y;
	}
	else {
		inputString.clear();
		mouseScrollDeltaX = 0;
		mouseScrollDeltaY = 0;
	}
}