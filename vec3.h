#pragma once

#include <cmath>
#include <iostream>
#include <print>
#include <format>

struct vec3 {
    double e[3] = { 0.0, 0.0, 0.0 };

    constexpr vec3() = default;
    constexpr vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

    constexpr double x() const { return e[0]; }
    constexpr double y() const { return e[1]; }
    constexpr double z() const { return e[2]; }

    constexpr vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    // double operator[](int i) const { return e[i]; }
    // double& operator[](int i) { return e[i]; }
    // template<typename Self>
    constexpr auto&& operator[](this vec3&& self, int i) {
        return self.e[i];
    }

    constexpr vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    constexpr vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    constexpr vec3& operator/=(double t) {
        return *this *= 1 / t;
    }
    constexpr double length() const {
        return std::sqrt(length_squared());
    }
    constexpr double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    // Return a random vector
    static constexpr vec3 random()
    {
        return vec3(random_double(), random_double(), random_double());
    }

    static constexpr vec3 random(double min, double max)
    {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

using point3 = vec3;

// Vector Utility Functions
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

// Specialize std::formatter for vec3
// TODO: Extend it to support colors
template<>
struct std::formatter<vec3> {
    // Parse the format specifiers (e.g., "{:spec}")
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    // Define how to write the object to the output context
    auto format(const vec3& v, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{} {} {}", v.e[0], v.e[1], v.e[2]);
    }
};

// constexpr is implicitly inline
constexpr vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

constexpr vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

// Element-wise multiplication
constexpr vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

constexpr vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

constexpr vec3 operator*(const vec3& v, double t) {
    return t * v;
}

constexpr vec3 operator/(const vec3& v, double t) {
    return (1 / t) * v;
}

constexpr double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
     + u.e[1] * v.e[1]
     + u.e[2] * v.e[2];
}

constexpr vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

constexpr vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

constexpr vec3 random_unit_vector()
{
    while (true)
    {
        auto p = vec3::random(-1, 1);
        auto lensq = p.length_squared();

        if (1E-160 < lensq && lensq <= 1)
        {
            return p / sqrt(lensq); 
        }
    }
}

constexpr vec3 random_on_hemisphere(const vec3& normal)
{
    vec3 on_unit_sphere = random_unit_vector();

    if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
    {
        return on_unit_sphere;
    }
    else
    {
        return -on_unit_sphere;
    }
}
