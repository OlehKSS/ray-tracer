#pragma once

#include <limits>

struct interval
{
    double min = +std::numeric_limits<double>::infinity();
    double max = -std::numeric_limits<double>::infinity();

    interval() = default;
    interval(double min, double max)
        : min(min)
        , max(max)
    {}

    // Create the interval tightly enclosing the two input intervals.
    interval(const interval& a, const interval& b)
    {
        min = (a.min <= b.min) ? a.min : b.min;
        max = (a.max >= b.max) ? a.max : b.max;
    }

    double size() const
    {
        return max - min;
    }

    bool contains(double x) const
    {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const
    {
        return min < x && x < max;
    }

    double clamp(double x) const 
    {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    // Pad an interval by a given amount delta
    interval expand(double delta) const
    {
        const auto padding = delta / 2;
        return interval(min - padding, max + padding);
    }

    static const interval empty;
    static const interval universe;
};

const interval interval::empty = interval(+std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
const interval interval::universe = interval(-std::numeric_limits<double>::infinity(), +std::numeric_limits<double>::infinity());

constexpr interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}

constexpr interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}
