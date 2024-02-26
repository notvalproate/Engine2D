#include "Core.hpp"

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
	SDL_SetCursor(SDL_CreateSystemCursor((SDL_SystemCursor)cursor));
}

bool CursorHandler::GetVisibility() const {
	return m_Visibility;
}

CursorLockMode CursorHandler::GetLockState() const {
	return m_LockState;
}