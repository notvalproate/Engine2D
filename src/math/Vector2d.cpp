#include "Vector2d.h"
#include <cmath>

Vector2d::Vector2d() 
	: x(0), y(0) { }

Vector2d::Vector2d(const double& p_x, const double& p_y)
	: x(p_x), y(p_y) { }

void Vector2d::Normalize() {
	if (x == 0 && y == 0) {
		return;
	}

	double k = std::sqrt(x * x + y * y);
	x /= k;
	y /= k;
}

void Vector2d::Scale(const int& p_Factor) {
	x *= p_Factor;
	y *= p_Factor;
}

double Vector2d::GetMagnitude() {
	return std::sqrt(x * x + y * y);
}
