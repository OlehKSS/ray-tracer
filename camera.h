#pragma once

#include <print>

#include "hittable.h"
#include "material.h"

class camera
{
public:
    double aspect_ratio = 1.0; // Ration fo image width over height
    int image_width = 100; // Rendered image width in pixel count
    int samples_per_pixel = 10; // Count of random samples for each pixel
    int max_depth = 10; // Maximum number of ray bounces into scene

    double vfov = 90; // Vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, 0); // Point camera is looking from
    point3 lookat = point3(0, 0, -1); // Point camera is looking at
    vec3 vup = vec3(0, 1, 0); // Camera relative "up" direction

    double defocus_angle = 0; // Variation angle in degrees of rays through each pixel
    double focus_distance = 10; // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable& world)
    {
        initialize();

        std::print("P3\n{}\n{}\n255\n", image_width, image_height);

        for (int j = 0; j < image_height; ++j)
        {
            std::print(std::clog, "\rScanlines remaining {} ", image_height - j);
            std::clog.flush();
            for (int i = 0; i < image_width; ++i)
            {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample)
                {
                    const auto r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }

        std::println(std::clog, "\rDone.                 ");
    }
private:
    void initialize()
    {
        image_height = std::max(1, static_cast<int>(image_width / aspect_ratio));
        pixel_samples_scale = 1.0 / samples_per_pixel;

        // Camera center
        center = lookfrom;

        // Determine viewport dimensions
        const double theta = degrees_to_radians(vfov);
        const double h = std::tan(theta / 2);
        const double viewport_height = 2 * h * focus_distance;
        const double viewport_width = viewport_height * (double(image_width) / image_height);

        // Calculate the u, v, w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat); // Opposite to the geometric model
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        
        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        const auto viewport_u = viewport_width * u; // Vector across viewport horizontal edge
        const auto viewport_v = viewport_height * (-v); // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        const auto viewport_upper_left = center - (focus_distance * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors
        auto defocus_radius = focus_distance * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    color ray_color(const ray& r, int depth, const hittable& world) const
    {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
        {
            return color(0, 0, 0);
        }

        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec))
        {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
            {
                return attenuation * ray_color(scattered, depth - 1, world);
            }
            return color(0, 0, 0);
        }
        const auto white = color(1.0, 1.0, 1.0);
        const auto blue = color(0.5, 0.7, 1.0);
        const auto unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * white + a * blue;
    }

    // Construct a camera ray originating from the defocus disk and directed at randomly sampled
    // point around the pixel location i, j.
    ray get_ray(int i, int j)
    {
        const auto offset = sample_square();
        const auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);
        const auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        const auto ray_direction = pixel_sample - ray_origin;
        const auto ray_time = random_double();
        return ray(ray_origin, ray_direction, ray_time);
    }

    // Return the vector to a random point in the [-0.5, -0.5]-[0.5, 0.5] unit square
    vec3 sample_square() const
    {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    // Return a random point in the camera defocus disk
    point3 defocus_disk_sample()
    {
        const auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    // TODO: Check Github for this function implementation
    vec3 sample_disk() const
    {
        return vec3();
    }

private:
    int image_height = 100; // Rendered image height
    double pixel_samples_scale; // Color nomalization factor for a sum of pixel samples
    point3 center; // Camera center
    point3 pixel00_loc; // Location of pixel 0, 0
    vec3 pixel_delta_u; // Offset to pixel to the right
    vec3 pixel_delta_v; // Offset to pixel below
    // Camera frame basis vectors
    vec3 u;
    vec3 v;
    vec3 w;
    vec3 defocus_disk_u; // Defocus disk horizontal radius
    vec3 defocus_disk_v; // Defocus disk vertical radius
};