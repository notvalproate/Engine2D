#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <string>
#include <cmath>
#include <algorithm>

#include <box2d/box2d.h>

#include "MathModule.hpp"
#include "Engine2DExport.hpp"

class Vector2D {
public:
    explicit inline constexpr Vector2D() = default;
    explicit inline constexpr Vector2D(double x, double y) : x(x), y(y) {}
    explicit inline Vector2D(const b2Vec2 box2dVec) : x(box2dVec.x), y(box2dVec.y) { }

    double x{}, y{};

    inline Vector2D& operator=(const b2Vec2 box2dVec) {
        x = box2dVec.x;
        y = box2dVec.y;
        return *this;
    }

    inline std::string ToString() const {
        return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
    }

    inline double GetMagnitude() const {
        return std::sqrt(x * x + y * y);
    }

    inline constexpr double GetMagnitudeSquared() const {
        return x * x + y * y;
    }

    inline Vector2D& Normalize() {
        if (x == 0 && y == 0) {
            return *this;
        }

        double k = std::sqrt(x * x + y * y);
        x /= k;
        y /= k;

        return *this;
    }
    
    inline Vector2D GetNormalized() const {
        return Vector2D(*this).Normalize();
    }

    inline constexpr Vector2D& Scale(const int factor) {
        x *= factor;
        y *= factor;

        return *this;
    }

    Vector2D& Rotate(const double angle) {
        const double angleRadians = angle * M_PI / 180.0;

        const double sinTheta = sin(angleRadians);
        const double cosTheta = cos(angleRadians);
        
        const double newX = x * cosTheta + y * sinTheta;
        const double newY = y * cosTheta - x * sinTheta;

        x = newX;
        y = newY;

        return *this;
    }
    
    inline Vector2D& RotateAround(const Vector2D& point, const double angle) {
        *this -= point;
        Rotate(angle);
        *this += point;

        return *this;
    }

    inline constexpr bool Equals(const Vector2D& other, const double epsil) const {
        return (MathModule::Abs(x - other.x) < epsil) && (MathModule::Abs(y - other.y) < epsil);
    }

    static inline constexpr double Cross(const Vector2D& v1, const Vector2D& v2) {
        return (v1.x * v2.y) - (v1.y * v2.x); 
    }

    static inline double Distance(const Vector2D& v1, const Vector2D& v2) {
        double dx = v2.x - v1.x;
        double dy = v2.y - v1.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    static inline constexpr double SquaredDistance(const Vector2D& v1, const Vector2D& v2) {
        double dx = v2.x - v1.x;
        double dy = v2.y - v1.y;
        return dx * dx + dy * dy;
    }

    static double Angle(const Vector2D& from, const Vector2D& to) {
        if (from == to) {
            return 0.0;
        }

        if (from == -to) {
            return 180.0;
        }

        double dot = from * to;

        if (dot == 0) {
            return 90.0;
        }

        double magnitudeFromTo = from.GetMagnitude() * to.GetMagnitude();

        if (magnitudeFromTo == 0.0) {
            return 0.0;
        }

        double cosAngle = std::clamp(dot / magnitudeFromTo, -1.0, 1.0);
        double angleRadians = std::acos(cosAngle);
        double angleDegrees = angleRadians * (180.0 / M_PI);

        return angleDegrees;
    }

    static double SignedAngle(const Vector2D& from, const Vector2D& to) {
        double angle = Angle(from, to);
        double crossProduct = Cross(from, to);
        angle *= MathModule::Sign(-crossProduct);

        return angle;
    }

    static inline Vector2D ClampMagnitude(Vector2D vec, const double maxLength) {
        double currentMagnitude = vec.GetMagnitude();

        if (currentMagnitude > maxLength && currentMagnitude > 0.0) {
            double scaleFactor = maxLength / currentMagnitude;
            vec.x *= scaleFactor;
            vec.y *= scaleFactor;
        }

        return vec;
    }

    static inline constexpr Vector2D Min(const Vector2D& v1, const Vector2D& v2) {
        return Vector2D(std::min(v1.x, v2.x), std::min(v1.y, v2.y));
    }
    
    static inline constexpr Vector2D Max(const Vector2D& v1, const Vector2D& v2) {
        return Vector2D(std::max(v1.x, v2.x), std::max(v1.y, v2.y));
    }

    static inline constexpr Vector2D Perpendicular(const Vector2D& vec) {
        return Vector2D(vec.y, -(vec.x));
    }

    static inline constexpr Vector2D Reflect(const Vector2D& inDirection, const Vector2D& inNormal) {
        double dotProduct = 2.0 * (inDirection * inNormal);
        double magnitudeSquared = inNormal.GetMagnitudeSquared();
        return inDirection - ((dotProduct / magnitudeSquared) * inNormal);
    }

    static inline constexpr Vector2D Lerp(const Vector2D& v1, const Vector2D& v2, double t) {
        t = std::clamp(t, 0.0, 1.0);
        return Vector2D(v1.x + (v2.x - v1.x) * t, v1.y + (v2.y - v1.y) * t);
    }
    
    static inline constexpr Vector2D LerpUnclamped(const Vector2D& v1, const Vector2D& v2, double t) {
        return Vector2D(v1.x + (v2.x - v1.x) * t, v1.y + (v2.y - v1.y) * t);
    }

    static Vector2D MoveTowards(const Vector2D& current, const Vector2D& target, double maxDistanceDelta) {
        Vector2D vectorToTarget = target - current;

        double distanceSquared = vectorToTarget.GetMagnitudeSquared();

        if (distanceSquared == 0.0 || (maxDistanceDelta >= 0 && distanceSquared <= maxDistanceDelta * maxDistanceDelta)) {
            return target;
        }

        double distance = std::sqrt(distanceSquared);

        return current + ((vectorToTarget * maxDistanceDelta) / distance);
    }

    static Vector2D SmoothDamp(
        const Vector2D& current,
        const Vector2D& target,
        Vector2D& currentVelocity,
        double smoothTime,
        double deltaTime,
        double maxSpeed = std::numeric_limits<double>::infinity()
        )
    {
        if (current == target) {
            currentVelocity = Vector2D::zero;
            return target;
        }

        double omega = 2.0 / smoothTime;
        double x = omega * deltaTime;
        double exp = 1.0 / (1.0 + x + 0.48 * x * x + 0.235 * x * x * x);

        Vector2D change = current - target;
        Vector2D tempVelocity = (currentVelocity - change * omega) * deltaTime;
        Vector2D newVelocity = (currentVelocity - tempVelocity * omega) * exp;

        Vector2D output = target + (change + tempVelocity) * exp;
        if (maxSpeed != std::numeric_limits<double>::infinity()) {
            Vector2D outputDir = output - target;
            double outputSpeed = outputDir.GetMagnitude();
            if (outputSpeed > maxSpeed) {
                outputDir = outputDir.GetNormalized() * maxSpeed;
                output = target + outputDir;
                newVelocity = (output - target) / deltaTime;
            }
        }

        currentVelocity = newVelocity;

        return output;
    }

    static constexpr double epsilon = 1e-4;

    inline constexpr Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    inline constexpr Vector2D& operator+=(const Vector2D& other) {
        x += other.x;
        y += other.y;

        return *this;
    }

    inline constexpr Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    inline constexpr Vector2D& operator-=(const Vector2D& other) {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    inline constexpr Vector2D operator-() const {
        return Vector2D(-x, -y);
    }

    inline constexpr double operator*(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }

    inline constexpr Vector2D operator*(const double k) const {
        return Vector2D(x * k, y * k);
    }

    friend inline constexpr Vector2D operator*(const double k, const Vector2D& vec) {
        return vec * k;
    }

    inline constexpr Vector2D& operator*=(const double k) {
        x *= k;
        y *= k;
        return *this;
    }

    inline constexpr Vector2D operator/(const double k) const {
        return Vector2D(x / k, y / k);
    }

    inline constexpr Vector2D& operator/=(const double k) {
        x /= k;
        y /= k;
        return *this;
    }

    inline constexpr bool operator==(const Vector2D& other) const {
        return (MathModule::Abs(x - other.x) < epsilon) && (MathModule::Abs(y - other.y) < epsilon);
    }

    inline constexpr bool operator!=(const Vector2D& other) const {
        return !(*this == other);
    }

    friend inline std::ostream& operator<<(std::ostream& os, const Vector2D& vec) {
        os << vec.ToString();
        return os;
    }

    static const ENGINE2D_API Vector2D up;
    static const ENGINE2D_API Vector2D down;
    static const ENGINE2D_API Vector2D left;
    static const ENGINE2D_API Vector2D right;
    static const ENGINE2D_API Vector2D one;
    static const ENGINE2D_API Vector2D zero;
};