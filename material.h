#pragma once

#include "hittable.h"
#include "texture.h"
#include "onb.h"

class material
{
public:
    virtual ~material() = default;

    virtual color emitted(double u, double v, const point3& p) const
    {
        return color(0, 0, 0);
    }

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf) const
    {
        return false;
    }

    virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
    {
        return 0;
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

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf) const override
    {
        onb uvw(rec.normal);
        auto scatter_direction = uvw.transform(random_cosine_direction());        
        scattered = ray(rec.p, unit_vector(scatter_direction), r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        pdf = dot(uvw.w(), scattered.direction()) / pi;
        return true;
    }

    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override
    {
        auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
        return cos_theta < 0 ? 0 : cos_theta / pi; // Discard rays in lower hemisphere
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

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf) const override
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

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf) const override
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

class diffuse_light : public material
{
public:
    diffuse_light(std::shared_ptr<texture> tex) : tex(tex) {}
    diffuse_light(const color& emit) : tex(std::make_shared<solid_color>(emit)) {}

    color emitted(double u, double v, const point3& p) const override
    {
        return tex->value(u, v, p);
    }

private:
    std::shared_ptr<texture> tex;
};

class isotropic : public material
{
public:
    isotropic(const color& albedo) : tex(std::make_shared<solid_color>(albedo)) {}
    isotropic(std::shared_ptr<texture> tex) : tex(tex) {}
    // The scattering function of isotropic picks a uniform random direction
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf) const override
    {
        scattered = ray(rec.p, random_unit_vector(), r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        pdf = 1 / (4 * pi);
        return true;
    }

    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override
    {
        return 1 / (4 * pi);
    }

private:
    std::shared_ptr<texture> tex;
};
