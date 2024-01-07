#include "Vector2d.h"
#include <cmath>

namespace notval {

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
