#pragma once

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable
{
public:
    // Static Sphere
    sphere(const point3& static_center, double radius, std::shared_ptr<material> mat)
        : center(static_center, vec3(0, 0, 0))
        , radius(std::fmax(0, radius))
        , mat(mat)
    {
        const auto rvec = vec3(radius, radius, radius);
        bbox = aabb(static_center - rvec, static_center + rvec);
    }

    // Moving Sphere
    sphere(const point3& center1, const point3& center2, double radius, std::shared_ptr<material> mat)
        : center(center1, center2 - center1)
        , radius(std::fmax(0, radius))
        , mat(mat)
    {
        // For a moving sphere, we want the bounds of its entire range of motion.
        const auto rvec = vec3(radius, radius, radius);
        aabb box1(center.at(0) - rvec, center.at(0) + rvec);
        aabb box2(center.at(1) - rvec, center.at(1) + rvec);
        bbox = aabb(box1, box2);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override 
    {
        const auto current_center = center.at(r.time());
        const auto oc = current_center - r.origin();
        const auto a = r.direction().length_squared();
        const auto h = dot(r.direction(), oc);
        const auto c = oc.length_squared() - radius * radius;

        const auto discriminant = h * h - a * c;
        if (discriminant < 0)
        {
            return false;
        }

        const auto sqrtd = std::sqrt(discriminant);
        // Find the nearest root that lies in the acceptable range
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root))
        {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
            {
                return false;
            }
        }
        rec.t = root;
        rec.p = r.at(rec.t);
        const auto outward_normal = (rec.p - current_center) / radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat = mat;
        return true;
    }

private:
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
    static void get_sphere_uv(const point3& p, double& u, double& v)
    {
        const auto theta = std::acos(-p.y());
        const auto phi = std::atan2(-p.z(), p.x()) + pi;
        u = phi / (2 * pi);
        v = theta / pi;
    }

private:
    ray center;
    double radius;
    std::shared_ptr<material> mat;
    aabb bbox;
};