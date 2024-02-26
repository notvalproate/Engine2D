#include "Core.hpp"

void CursorHandler::SetVisibility(bool visible) {
	SDL_ShowCursor(visible);
}