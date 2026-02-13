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

    static const interval empty;
    static const interval universe;
};

const interval interval::empty = interval(+std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
const interval interval::universe = interval(-std::numeric_limits<double>::infinity(), +std::numeric_limits<double>::infinity());
