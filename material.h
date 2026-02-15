#pragma once

#include "hittable.h"

class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const
    {
        return false;
    }
};

class lambertian : public material
{
public:
lambertian(const color& albedo)
    : albedo(albedo)
{
}

bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
{
    // Randomly generating a vector according to Lambertian distribution
    auto scatter_direction = rec.normal + random_unit_vector();

    // Catch degenerate scatter direction
    if (scatter_direction.near_zero())
    {
        scatter_direction = rec.normal;
    }

    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
}

private:
    color albedo; // Albedo is used to define some form of fractional reflectance
};

class metal : public material
{
public:
    // We randomize the reflected direction by using a small sphere centered around the original endpoint
    // and choosing a new endpoint for the ray. fuzz is equal to the radius of the additional sphere.
    metal(const color& albedo, double fuzz)
        : albedo(albedo)
        , fuzz(fuzz < 1 ? fuzz : 1)
    {

    }

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0); // Catch scattering below the surface
    }

private:
    color albedo;
    double fuzz;
};
