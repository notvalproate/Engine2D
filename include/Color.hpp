#pragma once

#include <algorithm>
#include <string>
#include <cmath>

namespace engine2d {

class Color {
public:
    explicit constexpr inline Color() = default;

    explicit constexpr inline Color(const float r, const float g, const float b)
        : 
        r(std::clamp(r, 0.0f, 1.0f)),
        g(std::clamp(g, 0.0f, 1.0f)),
        b(std::clamp(b, 0.0f, 1.0f)),
        a(1.0f) 
    { }

    explicit constexpr inline Color(const float r, const float g, const float b, const float a) 
        : 
        r(std::clamp(r, 0.0f, 1.0f)),
        g(std::clamp(g, 0.0f, 1.0f)),
        b(std::clamp(b, 0.0f, 1.0f)),
        a(std::clamp(a, 0.0f, 1.0f))
    { }

    inline std::string ToString() const {
        return "(" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + "," + std::to_string(a) + ")";
    }

    inline Color GetGamma() const {
        return Color(
            std::pow(r, 1.0f / gamma),
            std::pow(g, 1.0f / gamma),
            std::pow(b, 1.0f / gamma),
            a
        );
    }

    inline Color GetLinear() const {
        return Color(
            std::pow(r, gamma),
            std::pow(g, gamma),
            std::pow(b, gamma),
            a
        );
    }

    inline constexpr float GetGrayscale() const {
        return (0.299f * r) + (0.587f * g) + (0.114f * b);
    }

    inline constexpr float GetMaxColorComponent() const {
        return std::max({r, g, b});
    }

    static void RGBtoHSV(const Color& color, float& h, float& s, float& v) {
        float 
            min = std::min({color.r, color.g, color.b}), 
            max = std::max({color.r, color.g, color.b}), 
            delta = max - min;

        v = max;
        s = 0;

        if (max != 0.0f) {
            s = delta / max;
        }

        if (delta == 0.0f) {
            s = 0.0f;
            h = 0.0f;
            return;
        }

        if (max == color.r) {
            h = std::fmod(60 * ((color.g - color.b) / delta) + 360.0f, 360.0f);
        } 
        else if (max == color.g) {
            h = std::fmod(60 * ((color.b - color.r) / delta) + 120.0f, 360.0f);
        }
        else {
            h = std::fmod(60 * ((color.r - color.g) / delta) + 240.0f, 360.0f);
        }
    }
    
    static Color HSVtoRGB(const float h, const float s, const float v) {
        float r{}, g{}, b{};

        float c = s * v;
        float x = c * (1 - static_cast<float>(std::fabs(static_cast<float>(std::fmod(h / 60.0f, 2) - 1))));

        int hPrime = ((int)h / 60) % 6;

        switch (hPrime) {
        case 0:
            r = c; g = x; b = 0;
            break;
        case 1:
            r = x, g = c, b = 0;
            break;
        case 2:
            r = 0; g = c; b = x;
            break;
        case 3:
            r = 0; g = x; b = c;
            break;
        case 4:
            r = x; g = 0; b = c;
            break;
        case 5:
            r = c; g = 0; b = x;
            break;
        default:
            r = 0; g = 0; b = 0;
            break;
        }

        float m = v - c;

        return Color(r + m, g + m, b + m);
    }

    static inline constexpr Color Lerp(const Color& c1, const Color& c2, float t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return Color(c1.r + (c2.r - c1.r) * t, c1.g + (c2.g - c1.g) * t, c1.b + (c2.b - c1.b) * t);
    }

    static inline constexpr Color LerpUnclamped(const Color& c1, const Color& c2, float t) {
        return Color(c1.r + (c2.r - c1.r) * t, c1.g + (c2.g - c1.g) * t, c1.b + (c2.b - c1.b) * t);
    }

    float r{}, g{}, b{}, a{};

    static constexpr float gamma = 2.2f;

    inline constexpr Color operator+(const Color& other) {
        return Color(
            r + other.r,
            g + other.g,
            b + other.b,
            a + other.a
        );
    }
    
    inline constexpr Color operator-(const Color& other) {
        return Color(
            r - other.r,
            g - other.g,
            b - other.b,
            a - other.a
        );
    }
    
    inline constexpr Color operator*(const Color& other) {
        return Color(
            r * other.r,
            g * other.g,
            b * other.b,
            a * other.a
        );
    }
    
    inline constexpr Color operator*(const float value) {
        return Color(
            r * value,
            g * value,
            b * value,
            a * value
        );
    }
    
    friend inline constexpr Color operator*(const float b, const Color& a) {
        return Color(
            a.r * b,
            a.g * b,
            a.b * b, 
            a.a * b
        );
    }

    inline constexpr Color operator/(const float value) {
        return Color(
            r / value,
            g / value,
            b / value,
            a / value
        );
    }

    inline constexpr float operator[](const short index) const {
        switch (index) {
        case 0:
            return r;
        case 1:
            return g;
        case 2:
            return b;
        case 3:
            return a;
        default:
            return 0.0f;
        }
    }

    friend inline std::ostream& operator<<(std::ostream& os, const Color& color) {
        os << color.ToString();
        return os;
    }

    static const Color white;
    static const Color gray;
    static const Color black;
    static const Color clear;

    static const Color red;
    static const Color green;
    static const Color blue;

    static const Color cyan;
    static const Color magenta;
    static const Color yellow;
};

} // namespace engine2d