#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.hpp"
#include "Material.hpp"
#include "Vector3.hpp"
#include "AABB.hpp"

class Sphere : public Hittable {
public:
    Sphere(Vector3 _center, double _radius, std::shared_ptr<Material> _material)
            : center(_center), radius(_radius), mat(_material) {

        auto rvec = Vector3(radius, radius, radius);
        bbox = AABB(center - rvec, center + rvec);
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& outRec) const override {
        Vector3 oc = r.origin() - center;
        auto a = r.direction().lengthSquared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.lengthSquared() - radius * radius;

        auto discriminant = half_b*half_b - a*c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        outRec.t = root;
        outRec.p = r.at(outRec.t);

        Vector3 outward_normal = (outRec.p - center) / radius;
        outRec.setFaceNormal(r, outward_normal);
        getSphereUV(outward_normal, outRec.u, outRec.v);
        outRec.mat = mat;

        return true;
    }

    AABB boundingBox() const override { return bbox; }

private:
    static void getSphereUV(const Vector3& p, double& u, double& v) {
        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + pi;
        u = phi / (2*pi);
        v = theta / pi;
    }


    Vector3 center;
    double radius;
    std::shared_ptr<Material> mat;
    AABB bbox;
};

#endif