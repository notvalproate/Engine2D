#pragma once

class Vector2d {
public:
	Vector2d();
	Vector2d(const double& p_x, const double& p_y);

	double x, y;

	void Normalize();
	void Scale(const int& p_Factor);
	double GetMagnitude();

	//Overloading basic operators
	Vector2d operator+(const Vector2d& o_V) {
		return Vector2d(x + o_V.x, y + o_V.y);
	}
	Vector2d operator-(const Vector2d& o_V) {
		return Vector2d(x - o_V.x, y - o_V.y);
	}
	double operator*(const Vector2d& o_V) {
		return x * o_V.x + y * o_V.y;
	}
	Vector2d operator/(const float& k) {
		return Vector2d(x / k, y / k);
	}
	Vector2d operator*(const float& k) {
		return Vector2d(x * k, y * k);
	}
};