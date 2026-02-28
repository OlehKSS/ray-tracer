#pragma once

#pragma once

#include "hittable.h"

// A primitive defining a triangle, where
// 1. Q, the starting corner.
// 2. u, a vector representing the first side. Q+u gives one of the corners adjacent to Q.
// 3. v, a vector representing the second side. Q+v gives the other corner adjacent to Q.
class triangle : public hittable
{
public:
    triangle(const point3& Q, const vec3& u, const vec3& v, std::shared_ptr<material> mat)
        : Q(Q)
        , u(u)
        , v(v)
        , mat(mat)
    {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);
        set_bounding_box();
    }

    // Computer the bounding box of all three vertices
    virtual void set_bounding_box()
    {
        auto bbox_diagonal1 = aabb(Q, Q + u);
        auto bbox_diagonal2 = aabb(Q, Q + v);
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
        auto denom = dot(normal, r.direction());

        // No hit if the ray is parallel to the plane
        if (std::fabs(denom) < 1E-8)
        {
            return false;
        }

        // Return false if the hit point parameter t is outside the ray interval
        auto t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
        {
            return false;
        }

        // Determine if the hit point lies within the planar shape using its plane coordinates.
        auto intersection = r.at(t);
        vec3 planar_hitpt_vector = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
        {
            return false;
        }
        // Ray hits the 2D shape; set the rest of the hit record and return true.
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }
    // Given the hit point in plane coordinats, return false if it is outside the 
    // primitive, otherwise set the hit record UV coordinates and return true.
    virtual bool is_interior(double a, double b, hit_record& rec) const
    {
        if (a > 0 && b > 0 && a + b < 1)
        {
            rec.u = a;
            rec.v = b;
            return true;
        }

        return false;
    }

private:
    point3 Q;
    vec3 u;
    vec3 v;
    vec3 w;
    std::shared_ptr<material> mat;
    aabb bbox;
    vec3 normal; // A unit vecotr perpendicular to the triangle plane
    double D; // Implicit equation of a plane n*(x, y, z) = Ax+By+Cz=D
};
