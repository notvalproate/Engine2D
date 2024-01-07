#pragma once

namespace notval {

	class Vector2d {
	public:
		Vector2d() = default;
		Vector2d(const double px, const double py);

		double x{}, y{};

		void Normalize();
		void Scale(const int factor);
		double GetMagnitude() const;

		//Overloading basic operators
		Vector2d operator+(const Vector2d& other) const {
			return Vector2d(x + other.x, y + other.y);
		}
		Vector2d operator-(const Vector2d& other) const {
			return Vector2d(x - other.x, y - other.y);
		}
		double operator*(const Vector2d& other) const {
			return x * other.x + y * other.y;
		}
		Vector2d operator/(const float k) const {
			return Vector2d(x / k, y / k);
		}
		Vector2d operator*(const float k) const {
			return Vector2d(x * k, y * k);
		}
	};

}
