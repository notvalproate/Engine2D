#include "Rectangle.hpp"
#include <cmath>
#include <algorithm>

void RectUtil::Translate(SDL_Rect& Rectangle, const Vector2d& p_Translation) {
	//Add translation vector to current position;
	Rectangle.x += std::round(p_Translation.x);
	Rectangle.y += std::round(p_Translation.y);
}

void RectUtil::Scale(SDL_Rect& Rectangle, const float& s_x, const float& s_y) {
	//Multiply the width and height to respective scale factors
	Rectangle.w = std::round(Rectangle.w * s_x);
	Rectangle.h = std::round(Rectangle.h * s_y);
}

bool RectUtil::RectOverlapRect(const SDL_Rect& R1, const SDL_Rect& R2) {
	//Check for overlapping of rectangles
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
	//Calculate the near and far intersection points as ratio from 0 to 1.
	//The X of rectangle - X of ray origin divided by length of the ray to get the near ratio, same for Y
	Vector2d t_Near(
		(Rectangle.x - p_RayOrigin.x) / p_RayDir.x, 
		(Rectangle.y - p_RayOrigin.y) / p_RayDir.y
	);
	//Same with far except take X of rectangle + Width, same for Y, add height instead
	Vector2d t_Far(
		(Rectangle.x + Rectangle.w - p_RayOrigin.x) / p_RayDir.x,
		(Rectangle.y + Rectangle.h - p_RayOrigin.y) / p_RayDir.y
	);

	//If any of the ratios are not a number (divided by 0) return false
	if (std::isnan(t_Far.y) || std::isnan(t_Far.x)) return false;
	if (std::isnan(t_Near.y) || std::isnan(t_Near.x)) return false;

	//Temp variable to swap
	double t_Temp;

	//If near and far are in wrong direction, swap them
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

	//if near x is not less than far y or vice versa, no intersection of ray
	if (t_Near.x > t_Far.y || t_Near.y > t_Far.x) return false;
	//If any of the far time is negative, it didnt hit since the ray is ahead of the rectangle in that case
	if (std::min(t_Far.x, t_Far.y) < 0)	return false;

	//Time taken to hit is the greater value since the point closer to the tip of the ray is going to hit first
	p_TimeHitNear = std::max(t_Near.x, t_Near.y);
	
	//If the time to hit is > 1, the ray didnt actually intersect, the extension of the ray intersects
	if (p_TimeHitNear > 1) return false;

	//get contact point by putting the time taken to hit back into original equation for t_near
	p_ContactPoint.x = p_RayOrigin.x + (p_RayDir.x * p_TimeHitNear);
	p_ContactPoint.y = p_RayOrigin.y + (p_RayDir.y * p_TimeHitNear);

	//Get the normal
	//If nearx is greater, it intersects in x direction
	if (t_Near.x > t_Near.y) {
		//Normal in y direction is 0
		p_ContactNormal.y = 0;
		//if direction is negative, the ray is coming in from the right to the left. hence normal is left to right, x = 1
		if (p_RayDir.x < 0) {
			p_ContactNormal.x = 1;
		}
		//else ray is left to right, x = -1
		else {
			p_ContactNormal.x = -1;
		}
	}
	//if nearx is less, then it intersects in y direction
	else if (t_Near.x < t_Near.y) {
		//Normal in x direction is 0
		p_ContactNormal.x = 0;
		//Same logic as for x
		if (p_RayDir.y < 0) {
			p_ContactNormal.y = 1;
		}
		else {
			p_ContactNormal.y = -1;
		}
	}

	//If everything executes return true
	return true;
}


bool RectUtil::DynamicRectIntersectRect(const SDL_FRect& p_DynamicRect, const SDL_Rect& p_StaticRect, const Vector2d& p_CurrVelocity, Vector2d& p_ContactPoint, Vector2d& p_ContactNormal, double& p_TimeHitNear, const float& p_DeltaTime) {
	//If velocity is 0, dont check for collision
	if (p_CurrVelocity.x == 0 && p_CurrVelocity.y == 0) {
		return false;
	}
	
	//Expand the rectangle with width and height of dynamic rect. and move the x and y towards the top left accordingly to keep it centered
	SDL_FRect t_ExpandedRect = {  
		(float)p_StaticRect.x - (p_DynamicRect.w / (float)2.0),
		(float)p_StaticRect.y - (p_DynamicRect.h / (float)2.0),
		(float)p_StaticRect.w + p_DynamicRect.w,
		(float)p_StaticRect.h + p_DynamicRect.h
	};

	//The ray origin is the center of the dynamic rectangle
	Vector2d t_RayOrigin(
		p_DynamicRect.x + (p_DynamicRect.w / (float)2.0), 
		p_DynamicRect.y + (p_DynamicRect.h / (float)2.0)
	);
	//The ray direction is the velocity vector scaled by the time factor
	Vector2d t_RayDirection(
		p_CurrVelocity.x * (p_DeltaTime / (float)1000.0),
		p_CurrVelocity.y * (p_DeltaTime / (float)1000.0)
	);

	//Check if the ray intersects with the expanded rectangle
	if (RayIntersectRect(t_RayOrigin, t_RayDirection, t_ExpandedRect, p_ContactPoint, p_ContactNormal, p_TimeHitNear)) {
		return true;
	}

	return false;
}