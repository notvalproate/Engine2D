#include "Core.hpp"

CursorHandler::CursorHandler() : m_Visibility(true), m_LockState(CursorLockMode::None), m_SystemCursors({}) { }

void CursorHandler::SetVisibility(bool visible) {
	SDL_ShowCursor(visible);
	m_Visibility = visible;
}

void CursorHandler::SetLockState(CursorLockMode lockMode) {
	SDL_Window* window = Object::Screen.m_Window;

	switch (lockMode) {
	case CursorLockMode::None:
		SDL_SetWindowGrab(window, SDL_FALSE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
		break;
	case CursorLockMode::Locked:
		SDL_SetWindowGrab(window, SDL_TRUE);
		SDL_SetRelativeMouseMode(SDL_TRUE);
		break;
	case CursorLockMode::Confined:
		SDL_SetWindowGrab(window, SDL_TRUE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
		break;
	default:
		SDL_SetWindowGrab(window, SDL_FALSE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
		lockMode = CursorLockMode::None;
		break;
	}

	m_LockState = lockMode;
};

void CursorHandler::SetCursor(SystemCursor cursor) {
	if ((uint8)cursor >= (uint8)SystemCursor::SystemCursorCount) {
		cursor = SystemCursor::Arrow;
	}

	SDL_SetCursor(m_SystemCursors[(uint8)cursor]);
}

bool CursorHandler::GetVisibility() const {
	return m_Visibility;
}

CursorLockMode CursorHandler::GetLockState() const {
	return m_LockState;
}

void CursorHandler::InitSystemCursors() {
	m_SystemCursors = {
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO),
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND),
	};
}