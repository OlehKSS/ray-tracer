cmake_minimum_required(VERSION 3.20)
project(ray_tracer)

set(CMAKE_CXX_STANDARD 23)

add_executable(ray_tracer main.cpp)

target_precompile_headers(ray_tracer
    PRIVATE
        <cstdlib>
        <filesystem>
        <iostream>
        <limits>
        <print>
        <string>
        <vector>
        "external/stb_image.h"
)
