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
		Vector2d& operator+=(const Vector2d& other) {
			x += other.x;
			y += other.y;

			return *this;
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

		//Shorthands
		static const Vector2d forward;
		static const Vector2d back;
		static const Vector2d up;
		static const Vector2d down;
		static const Vector2d one;
		static const Vector2d zero;
	};

}
