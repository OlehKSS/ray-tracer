#pragma once

#ifdef _MSC_VER
    #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "external/stb_image.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <print>
#include <vector>

class rwt_image
{
public:
    rwt_image() = default;
    // Load image data from the specified file.
    // If the RTW_IMAGES environment variable is defined,
    // looks only in that directory for the image file. 
    rwt_image(std::string_view image_filename)
    {
        std::filesystem::path image_path(image_filename);

        if (auto imagedir = std::getenv("RTW_IMAGES"))
        {
            const auto status = load(std::filesystem::path(imagedir) / image_path);
            if (status) return;
        }

        if (load(image_path)) return;
        if (load("../images" / image_path)) return;

        std::println(std::cerr, "ERROR: Could not load image file {}", image_path.string());
    }

    ~rwt_image()
    {
        STBI_FREE(fdata);
    }

    bool load(const std::filesystem::path& filename)
    {
        auto n = bytes_per_pixel; // Dummy out parameter: original components per pixel
        fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
        if (fdata == nullptr) return false;

        bytes_per_scanline = image_width * bytes_per_pixel;
        convert_to_bytes();
        return true;
    }

    int width()  const { return (fdata == nullptr) ? 0 : image_width; }
    int height() const { return (fdata == nullptr) ? 0 : image_height; }

    // Return the address of the three RGB bytes of the pixel at x,y. If there is no image
    // data, returns magenta.
    const uint8_t* pixel_data(int x, int y) const
    {
        static uint8_t magenta[] = { 255, 0, 255 };
        if (bdata.empty()) return magenta;

        x = clamp(x, 0, image_width);
        y = clamp(y, 0, image_height);
        // Pixel is an implicit array of length 3
        return bdata.data() + y * bytes_per_scanline + x * bytes_per_pixel;
    }

private:
    // Return the value clamped to the range [low, high).
    static int clamp(int x, int low, int high)
    {
        if (x < low) return low;
        if (x < high) return x;
        return high - 1;
    }

    static uint8_t float_to_byte(float value)
    {
        if (value <= 0) return 0;
        if (1.0 <= value) return 255;
        return static_cast<uint8_t>(256 * value);
    }

    // Convert the linear floating point pixel data to bytes, storing the resulting byte
    // data in the `bdata` member.
    void convert_to_bytes()
    {
        const size_t total_bytes = image_width * image_height * bytes_per_pixel;
        bdata = std::vector<uint8_t>(total_bytes);

        auto* fptr = fdata;
        for (size_t i = 0; i < total_bytes; ++i, ++fptr)
        {
            bdata[i] = float_to_byte(*fptr);
        }
    }

    const int bytes_per_pixel = 3;
    float* fdata; // Linear floating point pixel data
    std::vector<uint8_t> bdata; // Linear 8-bit pixel data
    int image_width = 0;
    int image_height = 0;
    int bytes_per_scanline = 0;
};

// Restore MSVC compiler warnings
#ifdef _MSC_VER
    #pragma warning (pop)
#endif