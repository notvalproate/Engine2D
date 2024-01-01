#include "Rectangle.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

void RectUtil::Translate(SDL_Rect& rectangle, const Vector2d& translation) {
	rectangle.x += std::round(translation.x);
	rectangle.y += std::round(translation.y);
}

void RectUtil::Scale(SDL_Rect& rectangle, const float x, const float y) {
	rectangle.w = std::round(rectangle.w * x);
	rectangle.h = std::round(rectangle.h * y);
}

bool RectUtil::RectOverlapRect(const SDL_Rect& R1, const SDL_Rect& R2) {
	//Condition for overlapping of rectangles
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

bool RectUtil::RayIntersectRect(const Vector2d& rayOrigin, const Vector2d& rayDir, const SDL_FRect& rectangle, Vector2d& contactPoint, Vector2d& contactNormal, double& timeHitNear) {
	//Calculate the near and far intersection points as ratio from 0 to 1.
	Vector2d t_Near(
		(rectangle.x - rayOrigin.x) / rayDir.x, 
		(rectangle.y - rayOrigin.y) / rayDir.y
	);
	//For far intersection points, added width and height
	Vector2d t_Far(
		(rectangle.x + rectangle.w - rayOrigin.x) / rayDir.x,
		(rectangle.y + rectangle.h - rayOrigin.y) / rayDir.y
	);

	if (std::isnan(t_Far.y) || std::isnan(t_Far.x)) return false;
	if (std::isnan(t_Near.y) || std::isnan(t_Near.x)) return false;

	//If near and far are in wrong direction, swap them
	if (t_Near.x > t_Far.x) {
		std::swap(t_Near.x, t_Far.x);
	}
	if (t_Near.y > t_Far.y) {
		std::swap(t_Near.y, t_Far.y);
	}

	//Condition for intersection not met
	if (t_Near.x > t_Far.y || t_Near.y > t_Far.x) return false;

	//if far is negative, ray is ahead of rectangle
	if (std::min(t_Far.x, t_Far.y) < 0)	return false;

	//Ppoint closer to the tip of the ray is going to hit first, hence max
	timeHitNear = std::max(t_Near.x, t_Near.y);
	
	//If the time to hit is > 1, the extension of the ray intersects
	if (timeHitNear > 1) return false;

	//Get contact point from original equation
	contactPoint = Vector2d(rayOrigin + (rayDir * timeHitNear));

	//Get the normal
	//If nearx is greater, it intersects in x direction, otherwise y
	if (t_Near.x > t_Near.y) {
		//Normal in y direction is 0 and x is in opposite of ray direction
		contactNormal.y = 0; 
		contactNormal.x = -1 * rayDir.x / std::abs(rayDir.x); 
	}
	else if (t_Near.x < t_Near.y) {
		contactNormal.x = 0; 
		contactNormal.y = -1 * rayDir.y / std::abs(rayDir.y);
	}

	return true;
}



bool RectUtil::DynamicRectIntersectRect(const SDL_FRect& dynamicRect, const SDL_Rect& staticRect, const Vector2d& currVelocity, Vector2d& contactPoint, Vector2d& contactNormal, double& timeHitNear, const float deltaTime) {
	if (currVelocity.x == 0 && currVelocity.y == 0) {
		return false;
	}
	
	//Expand the rectangle with width and height of dynamic rect. Keeping centered
	SDL_FRect t_ExpandedRect = {  
		(float)staticRect.x - (dynamicRect.w / (float)2.0),
		(float)staticRect.y - (dynamicRect.h / (float)2.0),
		(float)staticRect.w + dynamicRect.w,
		(float)staticRect.h + dynamicRect.h
	};

	//The ray origin is the center
	Vector2d t_RayOrigin(
		dynamicRect.x + (dynamicRect.w / (float)2.0), 
		dynamicRect.y + (dynamicRect.h / (float)2.0)
	);
	Vector2d t_RayDirection(
		currVelocity.x * deltaTime,
		currVelocity.y * deltaTime
	);

	//Check if the ray intersects with the expanded rectangle
	if (RayIntersectRect(t_RayOrigin, t_RayDirection, t_ExpandedRect, contactPoint, contactNormal, timeHitNear)) {
		return true;
	}

	return false;
}
