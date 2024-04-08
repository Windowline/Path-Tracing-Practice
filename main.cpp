#include <iostream>
#include "src/rtweekend.h"
#include "src/color.h"
#include "src/camera.h"
#include "src/hittable.h"
#include "src/hittable_list.h"
#include "src/sphere.h"
#include "src/material.h"

vec3 ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, interval(0, infinity), rec)) {
        return 0.5 * (rec.normal + vec3(1, 1, 1));
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0 - a) * vec3(1.0, 1.0, 1.0) + a * vec3(0.5, 0.7, 1.0);
}



int main() {
    hittable_list world;

    auto material_ground = std::make_shared<lambertian>(vec3(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<lambertian>(vec3(0.1, 0.2, 0.5));
    auto material_left   = std::make_shared<dielectric>(1.5);
    auto material_right  = std::make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<sphere>(vec3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(vec3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<sphere>(vec3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(vec3(-1.0,    0.0, -1.0),  -0.4, material_left));
    world.add(make_shared<sphere>(vec3( 1.0,    0.0, -1.0),   0.5, material_right));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width  = 400;
    cam.samples_per_pixel = 50;
    cam.max_depth = 50;

    cam.vfov     = 20;
    cam.lookfrom = vec3(-2,2,1);
    cam.lookat   = vec3(0,0,-1);
    cam.vup      = vec3(0,1,0);

    cam.render(world);
}