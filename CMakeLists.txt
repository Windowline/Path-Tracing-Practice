cmake_minimum_required(VERSION 3.25)
project(Ray_Tracing_Basic)

set(CMAKE_CXX_STANDARD 17)

add_executable(main.cpp
        src/vec3.h
        src/color.h
        src/ray.h)
