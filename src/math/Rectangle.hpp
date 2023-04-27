#pragma once
#include "SDL.h"
#include "Vector2d.h"

namespace RectUtil {
	void Translate(SDL_Rect& Rectangle, const Vector2d& p_Translation);
	void Scale(SDL_Rect& Rectangle, const float& s_x, const float& s_y);
	bool RectOverlapRect(const SDL_Rect& R1, const SDL_Rect& R2);
	bool RayIntersectRect(const Vector2d& p_RayOrigin, const Vector2d& p_RayDir, const SDL_FRect& Rectangle, Vector2d& p_ContactPoint, Vector2d& p_ContactNormal, double& p_TimeHitNear);
	bool DynamicRectIntersectRect(const SDL_FRect& p_DynamicRect, const SDL_Rect& p_StaticRect, const Vector2d& p_CurrVelocity, Vector2d& p_ContactPoint, Vector2d& p_ContactNormal, double& p_TimeHitNear, const float& p_DeltaTime);
}