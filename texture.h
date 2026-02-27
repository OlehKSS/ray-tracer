#pragma once

#include <memory>

#include "color.h"
#include "vec3.h"
#include "perlin.h"
#include "rtw_image.h"

class texture
{
public:
    virtual ~texture() = default;
    virtual color value(double u, double v, const point3& p) const = 0;

};

class solid_color : public texture
{
public:
    solid_color(const color& albedo) : albedo(albedo) {}
    solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

    color value(double u, double v, const point3& p) const override
    {
        return albedo;
    }

private:
    color albedo;
};

class checker_texture : public texture
{
public:
    checker_texture(double scale, std::shared_ptr<texture> even, std::shared_ptr<texture> odd)
        : inv_scale(1.0 / scale)
        , even(even)
        , odd(odd)
    {}

    checker_texture(double scale, const color& c1, const color& c2)
        : checker_texture(scale, std::make_shared<solid_color>(c1), std::make_shared<solid_color>(c2))
    {}

    color value(double u, double v, const point3& p) const override
    {
        const auto xInteger = int(std::floor(inv_scale * p.x()));
        const auto yInteger = int(std::floor(inv_scale * p.y()));
        const auto zInteger = int(std::floor(inv_scale * p.z()));

        const bool isEven = (xInteger + yInteger + zInteger) % 2== 0;
        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    std::shared_ptr<texture> even;
    std::shared_ptr<texture> odd;
};

class image_texture : public texture
{
public:
    image_texture(std::string_view filename) : image(filename) {}

    color value(double u, double v, const point3& p) const override
    {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (image.height() <= 0) return color(0,1,1);

        u = interval(0, 1).clamp(u);
        v = 1.0 - interval(0, 1).clamp(v); // Flip V to image coordinates

        const auto i = static_cast<int>(u * image.width());
        const auto j = static_cast<int>(v * image.height());
        auto pixel = image.pixel_data(i, j);

        const auto color_scale = 1.0 / 255.0;
        return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    rwt_image image;
};

class noise_texture : public texture
{
public:
    noise_texture() = default;

    color value(double u, double v, const point3& p) const override
    {
        return color(1, 1, 1) * noise.noise(p);
    }

private:
    perlin noise;
};
