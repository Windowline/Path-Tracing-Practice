#ifndef HITTABLE_H
#define HITTABLE_H
#include "Interval.hpp"
#include "Ray.hpp"
#include "Utils.hpp"
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
        front_face = dot(r.direction(), outwardNormal) < 0;
        normal = front_face ? outwardNormal : -outwardNormal;
    }
};

class Hittable {
public:
    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& outRec) const = 0;

    virtual AABB boundingBox() const = 0;
};


class Translate : public Hittable {
public:
    Translate(shared_ptr<Hittable> object, const Vector3& offset)
            : object(object), offset(offset)
    {
        bbox = object->boundingBox() + offset;
    }


    bool hit(const Ray& r, Interval ray_t, HitRecord& outRec) const override {
        Ray offset_r(r.origin() - offset, r.direction(), r.time());
        if (!object->hit(offset_r, ray_t, outRec))
            return false;

        outRec.p += offset;

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
        sinTheta = sin(radians);
        cosTheta = cos(radians);
        bbox = object->boundingBox();

        Vector3 min(infinity, infinity, infinity);
        Vector3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx = cosTheta * x + sinTheta * z;
                    auto newz = -sinTheta * x + cosTheta * z;

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

    bool hit(const Ray& r, Interval ray_t, HitRecord& outRec) const override {
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cosTheta * r.origin()[0] - sinTheta * r.origin()[2];
        origin[2] = sinTheta * r.origin()[0] + cosTheta * r.origin()[2];

        direction[0] = cosTheta * r.direction()[0] - sinTheta * r.direction()[2];
        direction[2] = sinTheta * r.direction()[0] + cosTheta * r.direction()[2];

        Ray rotated_r(origin, direction, r.time());

        if (!object->hit(rotated_r, ray_t, outRec))
            return false;

        // Change the intersection point from object space to world space
        auto p = outRec.p;
        p[0] = cosTheta * outRec.p[0] + sinTheta * outRec.p[2];
        p[2] = -sinTheta * outRec.p[0] + cosTheta * outRec.p[2];

        // Change the normal from object space to world space
        auto normal = outRec.normal;
        normal[0] = cosTheta * outRec.normal[0] + sinTheta * outRec.normal[2];
        normal[2] = -sinTheta * outRec.normal[0] + cosTheta * outRec.normal[2];

        outRec.p = p;
        outRec.normal = normal;

        return true;
    }

    AABB boundingBox() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    double sinTheta;
    double cosTheta;
    AABB bbox;
};

#endif