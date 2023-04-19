#pragma once
#include "SDL.h"

namespace RectUtil {
	void Translate(SDL_Rect& Rectangle, const float& t_x, const float& t_y);
	void Scale(SDL_Rect& Rectangle, const float& s_x, const float& s_y);
	bool CheckCollision(const SDL_Rect& R1, const SDL_Rect& R2);
}