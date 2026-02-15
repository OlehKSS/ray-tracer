#pragma once

#include <iostream>
#include <cmath>

#include "vec3.h"
#include "interval.h"

using color = vec3;

constexpr double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
    {
        return std::sqrt(linear_component);
    }

    return 0;
}

void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Apply a linear to gamma transform for gamma 2 (Gamma correction)
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Translate the [0,1] component values to the byte range [0,255].
    static const interval intensity(0.0, 0.999);
    auto rbyte = static_cast<int>(256 * intensity.clamp(r));
    auto gbyte = static_cast<int>(256 * intensity.clamp(g));
    auto bbyte = static_cast<int>(256 * intensity.clamp(b));

    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}
