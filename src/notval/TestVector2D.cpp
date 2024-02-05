#include "Core.hpp"

#include <cmath>

const Vector2D Vector2D::up(0.0, -1.0);
const Vector2D Vector2D::down(0.0, 1.0);
const Vector2D Vector2D::left(-1.0, 0.0);
const Vector2D Vector2D::right(1.0, 0.0);
const Vector2D Vector2D::one(1.0, 1.0);
const Vector2D Vector2D::zero(0.0, 0.0);

Vector2D::Vector2D(double x, double y) : x(x), y(y) {}

double Vector2D::GetMagnitude() const {
    return std::sqrt(x * x + y * y);
}

double Vector2D::GetMagnitudeSquared() const {
    return x * x + y * y;
}

void Vector2D::Normalize() {
    if (x == 0 && y == 0) {
		return;
	}

	double k = std::sqrt(x * x + y * y);
	x /= k;
	y /= k;
}

void Vector2D::Scale(const int factor) {
    x *= factor;
	y *= factor;
}

Vector2D Vector2D::operator+(const Vector2D& other) const {
    return Vector2D(x + other.x, y + other.y);
}

Vector2D& Vector2D::operator+=(const Vector2D& other) {
    x += other.x;
    y += other.y;

    return *this;
}

Vector2D Vector2D::operator-(const Vector2D& other) const {
    return Vector2D(x - other.x, y - other.y);
}

Vector2D& Vector2D::operator-=(const Vector2D& other) {
    x -= other.x;
    y -= other.y;

    return *this;
}

double Vector2D::operator*(const Vector2D other) const {
    return x * other.x + y * other.y;
}

Vector2D Vector2D::operator*(const float k) const {
    return Vector2D(x * k, y * k);
}

Vector2D& Vector2D::operator*=(const float k) {
    x *= k;
    y *= k;
    return *this;
}

Vector2D Vector2D::operator/(const float k) const {
    return Vector2D(x / k, y / k);
}

Vector2D& Vector2D::operator/=(const float k) {
    x /= k;
    y /= k;
    return *this;
}

bool Vector2D::operator==(const Vector2D other) const {
    if ((std::abs(x - other.x) < 1e-5) && (std::abs(y - other.y) < 1e-5)) {
        return true;
    }
    return false;
}