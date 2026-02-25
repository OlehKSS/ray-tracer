#pragma once

#include "rtweekend.h"

class material;

struct hit_record
{
    point3 p;
    vec3 normal;
    std::shared_ptr<material> mat;
    double t;
    bool front_face;
    // Set the hit record normal vector
    void set_face_normal(const ray& r, const vec3& outward_normal)
    {
        const auto unit_outward_normal = unit_vector(outward_normal);
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;
    
    // Check if a ray hits this object between ray_t.min and ray_t.max interval
    // Return bool flag and hit_record
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
    virtual aabb bounding_box() const = 0;
};
