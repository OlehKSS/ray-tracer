#pragma once

#include "rtweekend.h"
#include "hittable.h"
#include "onb.h"

// Probability Density Distribution
class pdf
{
public:
    virtual ~pdf() = default;

    virtual double value(const vec3& direction) const = 0;
    virtual vec3 generate() const = 0;
};

class sphere_pdf : public pdf
{
public:
    sphere_pdf() = default;

    double value(const vec3& direction) const override
    {
        return 1 / (4 * pi);
    }

    vec3 generate() const override
    {
        return random_unit_vector();
    }
};

class cosine_pdf : public pdf
{
public:
    cosine_pdf(const vec3& w) : uvw(w) {}

    double value(const vec3& direction) const override
    {
        auto cosine_theta = dot(unit_vector(direction), uvw.w());
        return std::fmax(0, cosine_theta / pi);
    }

    vec3 generate() const override
    {
        return uvw.transform(random_cosine_direction());
    }

private:
    onb uvw;
};

class hittable_pdf : public pdf
{
public:
    hittable_pdf(const hittable& objects, const point3& origin)
        : objects(objects)
        , origin(origin)
    {

    }

    double value(const vec3& direction) const override
    {
        return objects.pdf_value(origin, direction);
    }

    vec3 generate() const override
    {
        return objects.random(origin);
    }

private:
    const hittable& objects;
    point3 origin;
};

class mixture_pdf : public pdf
{
public:
    mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1)
    {
        p[0] = p0;
        p[1] = p1;
    }

    double value(const vec3& direction) const override
    {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    vec3 generate() const override
    {
        if (random_double() < 0.5)
        {
            return p[0]->generate();
        }
        else
        {
            return p[1]->generate();
        }
    }

private:
    std::shared_ptr<pdf> p[2];
};

class power_cosine_pdf : public pdf
{
public:
    power_cosine_pdf(const vec3& reflection_direction, double exponent)
        : uvw(reflection_direction)
        , n(exponent)
    {
        
    }

    double value(const vec3& direction) const override
    {
        auto cos_alpha = dot(unit_vector(direction), uvw.w());
        cos_alpha = std::fmax(0, cos_alpha);
        return ((n + 1) / (2.0 * pi)) * std::pow(cos_alpha, n);
    }

    vec3 generate() const override
    {
        // Generate local angles (Power-Cosine)
        auto u = random_double();
        auto v = random_double();
        auto sin_phi = std::sin(2 * pi * u);
        auto cos_phi = std::cos(2 * pi * u);
        auto cos_theta = std::pow(v, 1 / (n + 1));
        auto sin_theta = std::sqrt(std::fmax(0.0, 1 - cos_theta * cos_theta));

        auto local_ray = vec3(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta);

        return uvw.transform(local_ray);
    }

private:
    onb uvw;
    double n;
};
