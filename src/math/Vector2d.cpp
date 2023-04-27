#include "Vector2d.h"
#include <cmath>

Vector2d::Vector2d() 
	: x(0), y(0) { }

Vector2d::Vector2d(const double& p_x, const double& p_y)
	: x(p_x), y(p_y) { }

void Vector2d::Normalize() {
	//If either x and y is 0, return;
	if (x == 0 && y == 0) {
		return;
	}
	//Get the magnitude and divide x and y by it
	double k = std::sqrt(x * x + y * y);
	x /= k;
	y /= k;
}

void Vector2d::Scale(const int& p_Factor) {
	//Scale the vector in same direction by a factor
	x *= p_Factor;
	y *= p_Factor;
}

double Vector2d::GetMagnitude() {
	//Return magnitude
	return std::sqrt(x * x + y * y);
}
