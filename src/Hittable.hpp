#ifndef HITTABLE_H
#define HITTABLE_H
#include "Interval.hpp"
#include "Ray.hpp"
#include "rtweekend.h"
#include "AABB.hpp"

#include <memory>

class Material;

class HitRecord {
public:
    Vector3 p;
    Vector3 normal;
    std::shared_ptr<Material> mat;
    double t;
    double u;
    double v;
    bool front_face;

    void setFaceNormal(const Ray& r, const Vector3& outwardNormal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = dot(r.direction(), outwardNormal) < 0;
        normal = front_face ? outwardNormal : -outwardNormal;
    }
};

class Hittable {
public:
    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;

    virtual AABB boundingBox() const = 0;
};


class Translate : public Hittable {
public:
    Translate(shared_ptr<Hittable> object, const Vector3& offset)
            : object(object), offset(offset)
    {
        bbox = object->boundingBox() + offset;
    }


    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        // Move the ray backwards by the offset
        Ray offset_r(r.origin() - offset, r.direction(), r.time());

        // Determine whether an intersection exists along the offset ray (and if so, where)
        if (!object->hit(offset_r, ray_t, rec))
            return false;

        // Move the intersection point forwards by the offset
        rec.p += offset;

        return true;
    }

    AABB boundingBox() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    Vector3 offset;
    AABB bbox;
};


class RotateY : public Hittable {
public:
    RotateY(shared_ptr<Hittable> object, double angle) : object(object) {
        auto radians = degrees2radians(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        bbox = object->boundingBox();

        Vector3 min(infinity, infinity, infinity);
        Vector3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;

                    Vector3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = AABB(min, max);
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        // Change the ray from world space to object space
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
        origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

        direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
        direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

        Ray rotated_r(origin, direction, r.time());

        // Determine whether an intersection exists in object space (and if so, where)
        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        // Change the intersection point from object space to world space
        auto p = rec.p;
        p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
        p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

        // Change the normal from object space to world space
        auto normal = rec.normal;
        normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
        normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

        rec.p = p;
        rec.normal = normal;

        return true;
    }

    AABB boundingBox() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    double sin_theta;
    double cos_theta;
    AABB bbox;
};

#endif