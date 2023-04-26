#pragma once

class Vector2d {
public:
	Vector2d();
	Vector2d(const double& p_x, const double& p_y);
	~Vector2d();

	double x, y;

	void Normalize();
	void Scale(const int& p_Factor);
	double GetMagnitude();

	Vector2d operator+(const Vector2d& o_V) {
		return Vector2d(x + o_V.x, y + o_V.y);
	}
	Vector2d operator-(const Vector2d& o_V) {
		return Vector2d(x - o_V.x, y - o_V.y);
	}
	Vector2d operator/(const int& k) {
		return Vector2d(x / k, y / k);
	}
};