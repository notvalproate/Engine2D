#pragma once

class Vector2d {
public:
	Vector2d();
	Vector2d(const double& p_x, const double& p_y);
	~Vector2d();

	double x, y;
};