#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

color ray_color(const ray& r, const hittable& world)
{
    hit_record rec;
    if (world.hit(r, interval(0, infinity), rec))
    {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }
    const auto white = color(1.0, 1.0, 1.0);
    const auto blue = color(0.5, 0.7, 1.0);
    const auto unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * white + a * blue;
}

int main()
{
    // Image
    constexpr double aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 400;
    constexpr int image_height = std::max(1, static_cast<int>(image_width / aspect_ratio));

    // World
    hittable_list world;

    world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera
    constexpr double focal_length = 1.0; // Distance from the camera to the viewport
    constexpr double viewport_height = 2.0;
    constexpr double viewport_width = viewport_height * (double(image_width) / image_height);
    const auto camera_center = point3(0, 0, 0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    const auto viewport_u = vec3(viewport_width, 0, 0);
    const auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    const auto pixel_delta_u = viewport_u / image_width;
    const auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    const auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    const auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


    // Render

    std::print("P3\n{}\n{}\n255\n", image_width, image_height);

    for (int j = 0; j < image_height; ++j)
    {
        std::print(std::clog, "\rScanlines remaining {} ", image_height - j);
        std::clog.flush();
        for (int i = 0; i < image_width; ++i)
        {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);
    
            auto pixel_color = ray_color(r, world);
            write_color(std::cout, pixel_color);
        }
    }

    std::println(std::clog, "\rDone.                 ");

    return 0;
}