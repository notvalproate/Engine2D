#pragma once

class Vector2d {
public:
	Vector2d() = default;
	Vector2d(const double& p_x, const double& p_y);

	double x{}, y{};
	
	void Normalize();
	void Scale(const int& p_Factor);
	double GetMagnitude() const;

	//Overloading basic operators
	Vector2d operator+(const Vector2d& o_V) const {
		return Vector2d(x + o_V.x, y + o_V.y);
	}
	Vector2d operator-(const Vector2d& o_V) const {
		return Vector2d(x - o_V.x, y - o_V.y);
	}
	double operator*(const Vector2d& o_V) const {
		return x * o_V.x + y * o_V.y;
	}
	Vector2d operator/(const float& k) const {
		return Vector2d(x / k, y / k);
	}
	Vector2d operator*(const float& k) const {
		return Vector2d(x * k, y * k);
	}
};