#pragma once

#include "interval.h"
#include "vec3.h"
#include "ray.h"

// Axis-Aligned Bounding Box
class aabb
{
public:
    static const aabb empty;
    static const aabb universe;

    interval x;
    interval y;
    interval z;

    aabb() = default;

    aabb(const interval& x, const interval& y, const interval& z)
        : x(x)
        , y(y)
        , z(z)
    {
        pad_to_minimums();
    }

    aabb(const point3& a, const point3& b)
    {
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
        // particular minimum/maximum coordinate order
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
    }

    aabb(const aabb& box0, const aabb& box1)
    {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
    }

    const interval& axis_interval(int n) const
    {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit(const ray& r, interval ray_t) const
    {
        auto&& ray_origin = r.origin();
        auto&& ray_direction = r.direction();

        for (int axis = 0; axis < 3; ++axis)
        {
            auto&& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_direction[axis];

            const auto t0 = (ax.min - ray_origin[axis]) * adinv;
            const auto t1 = (ax.max - ray_origin[axis]) * adinv;

            if (t0 < t1)
            {
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            }
            else
            {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 > ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
            {
                return false;
            }
        }

        return true;
    }

    // Return the index of the longest axis of the bounding box
    int longest_axis() const
    {
        if (x.size() > y.size())
        {
            return x.size() > z.size() ? 0 : 2;
        }
        else
        {
            return y.size() > z.size() ? 1 : 2; 
        }
    }

private:
    // Insert a small padding to ensure that newly constructed AABBs always have a non-zero volume
    void pad_to_minimums()
    {
        // Adjust the AABB so that no side is narrower than some delta, padding if necessary.
        constexpr double delta = 0.0001;
        if (x.size() < delta) x = x.expand(delta);
        if (y.size() < delta) y = y.expand(delta);
        if (z.size() < delta) z = z.expand(delta);
    }
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);
