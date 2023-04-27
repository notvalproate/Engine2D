#include "Rectangle.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

void RectUtil::Translate(SDL_Rect& Rectangle, const float& t_x, const float& t_y) {
	Rectangle.x += std::round(t_x);
	Rectangle.y += std::round(t_y);
}

void RectUtil::Scale(SDL_Rect& Rectangle, const float& s_x, const float& s_y) {
	Rectangle.w = std::round(Rectangle.w * s_x);
	Rectangle.h = std::round(Rectangle.h * s_y);
}

bool RectUtil::RectIntersectRect(const SDL_Rect& R1, const SDL_Rect& R2) {
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

bool RectUtil::RayIntersectRect(const Vector2d& p_RayOrigin, const Vector2d& p_RayDir, const SDL_FRect& Rectangle, Vector2d& p_ContactPoint, Vector2d& p_ContactNormal, double& p_TimeHitNear) {
	double t_Temp;

	Vector2d t_Near(
		(Rectangle.x - p_RayOrigin.x) / p_RayDir.x, 
		(Rectangle.y - p_RayOrigin.y) / p_RayDir.y
	);
	Vector2d t_Far(
		(Rectangle.x + Rectangle.w - p_RayOrigin.x) / p_RayDir.x,
		(Rectangle.y + Rectangle.h - p_RayOrigin.y) / p_RayDir.y
	);

	if (std::isnan(t_Far.y) || std::isnan(t_Far.x)) return false;
	if (std::isnan(t_Near.y) || std::isnan(t_Near.x)) return false;

	if (t_Near.x > t_Far.x) {
		t_Temp = t_Near.x;
		t_Near.x = t_Far.x;
		t_Far.x = t_Temp;
	}
	if (t_Near.y > t_Far.y) {
		t_Temp = t_Near.y;
		t_Near.y = t_Far.y;
		t_Far.y = t_Temp;
	}

	if (t_Near.x > t_Far.y || t_Near.y > t_Far.x) return false;

	p_TimeHitNear = std::max(t_Near.x, t_Near.y);
	
	if (p_TimeHitNear > 1) return false;
	if (std::min(t_Far.x, t_Far.y) < 0)	return false;

	p_ContactPoint.x = p_RayOrigin.x + (p_RayDir.x * p_TimeHitNear);
	p_ContactPoint.y = p_RayOrigin.y + (p_RayDir.y * p_TimeHitNear);

	if (t_Near.x > t_Near.y) {
		p_ContactNormal.y = 0;
		if (p_RayDir.x < 0) {
			p_ContactNormal.x = 1;
		}
		else {
			p_ContactNormal.x = -1;
		}
	}
	else if (t_Near.x < t_Near.y) {
		p_ContactNormal.x = 0;
		if (p_RayDir.y < 0) {
			p_ContactNormal.y = 1;
		}
		else {
			p_ContactNormal.y = -1;
		}
	}

	return true;
}


bool RectUtil::DynamicRectIntersectRect(const SDL_FRect& p_DynamicRect, const SDL_Rect& p_StaticRect, const Vector2d& p_CurrVelocity, Vector2d& p_ContactPoint, Vector2d& p_ContactNormal, double& p_TimeHitNear, const float& p_DeltaTime) {
	if (p_CurrVelocity.x == 0 && p_CurrVelocity.y == 0) {
		return false;
	}

	SDL_FRect t_ExpandedRect = { 
		(float)p_StaticRect.x - (p_DynamicRect.w / (float)2.0),
		(float)p_StaticRect.y - (p_DynamicRect.h / (float)2.0),
		(float)p_StaticRect.w + p_DynamicRect.w,
		(float)p_StaticRect.h + p_DynamicRect.h
	};

	Vector2d t_RayOrigin(p_DynamicRect.x + (p_DynamicRect.w / (float)2.0), p_DynamicRect.y + (p_DynamicRect.h / (float)2.0));
	Vector2d t_RayDirection(p_CurrVelocity.x * (p_DeltaTime / (float)1000.0), p_CurrVelocity.y * (p_DeltaTime / 1000.0));

	if (RayIntersectRect(t_RayOrigin, t_RayDirection, t_ExpandedRect, p_ContactPoint, p_ContactNormal, p_TimeHitNear)) {
		return true;
	}

	return false;
}