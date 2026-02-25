#pragma once

#include "hittable.h"
#include "texture.h"

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
        : tex(std::make_shared<solid_color>(albedo))
    {
    }
    lambertian(std::shared_ptr<texture> tex) 
        : tex(tex)
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

        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    std::shared_ptr<texture> tex; // Tex (Albedo) is used to define some form of fractional reflectance
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
        scattered = ray(rec.p, reflected, r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0); // Catch scattering below the surface
    }

private:
    color albedo;
    double fuzz;
};

class dielectric : public material
{
public:
    // A dielectic material that only refracts (when possible)
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1 / refraction_index) : refraction_index;
        const auto unit_direction = unit_vector(r_in.direction());
        const double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        const double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
        {
            direction = reflect(unit_direction, rec.normal);
        }
        else
        {
            direction = refract(unit_direction, rec.normal, ri);
        }

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }

private:
    static double reflectance(double cosine, double refraction_index)
    {
        // Glass (dielectric) reflectivity varies with angle.
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow(1 - cosine, 5);
    }

private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;
};
