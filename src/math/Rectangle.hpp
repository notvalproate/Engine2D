#pragma once
#include "SDL.h"
#include "Vector2d.h"

namespace RectUtil {
	void Translate(SDL_Rect& rectangle, const Vector2d& translation);
	void Scale(SDL_Rect& rectangle, const float x, const float y);
	bool RectOverlapRect(const SDL_Rect& R1, const SDL_Rect& R2);
	bool RayIntersectRect(const Vector2d& rayOrigin, const Vector2d& rayDir, const SDL_FRect& rectangle, Vector2d& contactPoint, Vector2d& contactNormal, double& timeHitNear);
	bool DynamicRectIntersectRect(const SDL_FRect& dynamicRect, const SDL_Rect& staticRect, const Vector2d& currVelocity, Vector2d& contactPoint, Vector2d& contactNormal, double& timeHitNear, const float deltaTime);
}