#include "Vector2d.h"
#include <cmath>

namespace notval {

	const Vector2d Vector2d::forward(1.0, 0.0);
	const Vector2d Vector2d::back(-1.0, 0.0);
	const Vector2d Vector2d::up(0.0, -1.0);
	const Vector2d Vector2d::down(0.0, 1.0);
	const Vector2d Vector2d::one(1.0, 1.0);
	const Vector2d Vector2d::zero(0.0, 0.0);

	Vector2d::Vector2d(const double px, const double py)
		: x(px), y(py) { }

	void Vector2d::Normalize() {
		if (x == 0 && y == 0) {
			return;
		}

		double k = std::sqrt(x * x + y * y);
		x /= k;
		y /= k;
	}

	void Vector2d::Scale(const int factor) {
		x *= factor;
		y *= factor;
	}

	double Vector2d::GetMagnitude() const {
		return std::sqrt(x * x + y * y);
	}

}
