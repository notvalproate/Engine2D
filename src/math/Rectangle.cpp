#include "Rectangle.hpp"
#include <cmath>

void RectUtil::Translate(SDL_Rect& Rectangle, const int& t_x, const int& t_y) {
	Rectangle.x += t_x;
	Rectangle.y += t_y;
}

void RectUtil::Scale(SDL_Rect& Rectangle, const float& s_x, const float& s_y) {
	Rectangle.w = std::round(Rectangle.w * s_x);
	Rectangle.h = std::round(Rectangle.h * s_y);
}

bool RectUtil::CheckCollision(const SDL_Rect& R1, const SDL_Rect& R2) {
	if (
		R1.x < R2.x + R2.w &&
		R1.x + R1.w > R2.x &&
		R1.y < R2.y + R2.h &&
		R1.h + R1.y > R2.y
		) {
		return true;
	}
	return false;
}