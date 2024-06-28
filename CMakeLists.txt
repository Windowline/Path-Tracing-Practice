cmake_minimum_required(VERSION 3.25)
project(Ray_Tracing_Advanced)

set(CMAKE_CXX_STANDARD 17)

add_executable(main.cpp
        src/Vector3.hpp
        src/Color.hpp
        src/Ray.hpp
        src/Hittable.hpp
        src/Sphere.hpp
        src/Hittable_list.hpp
        src/rtweekend.h
        src/Interval.hpp
        src/Camera.hpp
        src/Material.hpp
        src/AABB.hpp
        src/BVH.hpp
        src/Texture.hpp
        src/rtw_stb_image.h
        src/external/stb_image.h
        src/Quad.hpp
)
