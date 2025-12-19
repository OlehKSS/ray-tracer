#include <iostream>
#include <print>

#include "color.h"

int main()
{
    constexpr int image_width = 256;
    constexpr int image_height = 256;

    // Render

    std::print("P3\n{}\n{}\n255\n", image_width, image_height);

    for (int j = 0; j < image_height; ++j)
    {
        std::print(std::clog, "\rScanlines remaining {} ", image_height - j);
        std::clog.flush();
        for (int i = 0; i < image_width; ++i)
        {
            auto pixel_color = color(static_cast<double>(i) / (image_width - 1), 
                static_cast<double>(j) / (image_height - 1), 0.0);
            write_color(std::cout, pixel_color);
        }
    }

    std::println(std::clog, "\rDone.                 ");

    return 0;
}