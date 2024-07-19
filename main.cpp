#include <iostream>
#include "src/utils.h"
#include "src/Color.hpp"
#include "src/Camera.hpp"
#include "src/Hittable.hpp"
#include "src/HittableList.hpp"
#include "src/Sphere.hpp"
#include "src/Material.hpp"
#include "src/BVH.hpp"
#include "src/Texture.hpp"
#include "src/Quad.hpp"

inline shared_ptr<HittableList> box(const Vector3& a, const Vector3& b, shared_ptr<Material> mat)
{
    auto sides = make_shared<HittableList>();
    auto min = Vector3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    auto max = Vector3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

    auto dx = Vector3(max.x() - min.x(), 0, 0);
    auto dy = Vector3(0, max.y() - min.y(), 0);
    auto dz = Vector3(0, 0, max.z() - min.z());

    sides->add(make_shared<Quad>(Vector3(min.x(), min.y(), max.z()), dx, dy, mat)); // front
    sides->add(make_shared<Quad>(Vector3(max.x(), min.y(), max.z()), -dz, dy, mat)); // right
    sides->add(make_shared<Quad>(Vector3(max.x(), min.y(), min.z()), -dx, dy, mat)); // back
    sides->add(make_shared<Quad>(Vector3(min.x(), min.y(), min.z()), dz, dy, mat)); // left
    sides->add(make_shared<Quad>(Vector3(min.x(), max.y(), max.z()), dx, -dz, mat)); // top
    sides->add(make_shared<Quad>(Vector3(min.x(), min.y(), min.z()), dx, dz, mat)); // bottom

    return sides;
}

void cornellBox() {
    HittableList world;

    auto red   = make_shared<Lambertian>(Vector3(.65, .05, .05));
    auto white = make_shared<Lambertian>(Vector3(.73, .73, .73));
    auto green = make_shared<Lambertian>(Vector3(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Vector3(15, 15, 15));

    world.add(make_shared<Quad>(Vector3(555, 0, 0), Vector3(0, 555, 0), Vector3(0, 0, 555), green));
    world.add(make_shared<Quad>(Vector3(0, 0, 0), Vector3(0, 555, 0), Vector3(0, 0, 555), red));
    world.add(make_shared<Quad>(Vector3(343, 554, 332), Vector3(-130, 0, 0), Vector3(0, 0, -105), light));
    world.add(make_shared<Quad>(Vector3(0, 0, 0), Vector3(555, 0, 0), Vector3(0, 0, 555), white));
    world.add(make_shared<Quad>(Vector3(555, 555, 555), Vector3(-555, 0, 0), Vector3(0, 0, -555), white));
    world.add(make_shared<Quad>(Vector3(0, 0, 555), Vector3(555, 0, 0), Vector3(0, 555, 0), white));

//    world.add(box(vec3(130, 0, 65), vec3(295, 165, 230), white));
//    world.add(box(vec3(265, 0, 295), vec3(430, 330, 460), white));
    shared_ptr<Hittable> box1 = box(Vector3(0, 0, 0), Vector3(165, 330, 165), white);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vector3(265, 0, 295));
    world.add(box1);

    shared_ptr<Hittable> box2 = box(Vector3(0, 0, 0), Vector3(165, 165, 165), white);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vector3(130, 0, 65));
    world.add(box2);

    Camera cam;
    cam.aspectRatio = 1.0;
    cam.imgWidth = 400;
    cam.samplePerPixel = 50;
    cam.maxDepth = 50;
    cam.fovy = 30;
    cam.camPos = Vector3(278, 278, -800);
    cam.lookAt = Vector3(278, 278, 0);
    cam.up = Vector3(0, 1, 0);
    cam.background = Vector3(0, 0, 0);

    cam.render(world);
}

void bouncingSpheres() {
    HittableList world;
    auto checker = make_shared<CheckerTexture>(0.4, Vector3(0.0, 0.0, 0.0), Vector3(1.0, 1.0, 1.0));
    world.add(make_shared<Sphere>(Vector3(0, -1000, 0), 1000, make_shared<Lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto chooseMat = randomDouble();
            Vector3 center(a + 0.9 * randomDouble(), 0.2, b + 0.9 * randomDouble());

            if ((center - Vector3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> sphereMaterial;

                if (chooseMat < 0.8) {
                    // metal
                    auto albedo = Vector3::random(0.1, 1);
                    auto fuzz = randomDouble(0, 0.6);
                    sphereMaterial = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, randomDouble(0.1, 0.3), sphereMaterial));
                } else if (chooseMat < 0.95) {
                    // diffuse
                    auto albedo = Vector3::random() * Vector3::random();
                    sphereMaterial = make_shared<Lambertian>(albedo);
                    world.add(make_shared<Sphere>(center, randomDouble(0.1, 0.3), sphereMaterial));

                } else {
                    // glass
                    sphereMaterial = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, randomDouble(0.1, 0.3), sphereMaterial));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(Vector3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(Vector3(0.2, 0.6, 0.15));
    world.add(make_shared<Sphere>(Vector3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(Vector3(0.7, 0.65, 0.55), 0.0);
    world.add(make_shared<Sphere>(Vector3(4, 1, 0), 1.0, material3));

    world = HittableList(make_shared<BVHNode>(world));

    Camera camera;

    camera.onSkyBackground = true;
    camera.aspectRatio = 16.0 / 9.0;

//    camera.image_width       = 1200;
//    camera.samples_per_pixel = 500;
//    camera.max_depth         = 50;
    camera.imgWidth = 400;
    camera.samplePerPixel = 16;
    camera.maxDepth = 4;

    camera.fovy = 25;
    camera.camPos = Vector3(-15, 4, 5);
    camera.lookAt = Vector3(0, 0, 0);
    camera.up = Vector3(0, 1, 0);

    camera.render(world);
}

int main() {
    bouncingSpheres();
//    cornellBox();
}
