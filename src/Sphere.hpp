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

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        Vector3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius*radius;

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

        rec.t = root;
        rec.p = r.at(rec.t);

        Vector3 outward_normal = (rec.p - center) / radius;
        rec.setFaceNormal(r, outward_normal);
        getSphereUV(outward_normal, rec.u, rec.v);
        rec.mat = mat;

        return true;
    }

    AABB boundingBox() const override { return bbox; }

private:
    static void getSphereUV(const Vector3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

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