#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.hpp"
#include "Material.hpp"
#include "Vector3.hpp"
#include "AABB.hpp"
#include "ONB.hpp"

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

    double pdfValue(const Vector3& origin, const Vector3& direction) const override {
        HitRecord rec;
        if (!this->hit(Ray(origin, direction), Interval(0.001, infinity), rec))
            return 0;

        auto cos_theta_max = std::sqrt(1 - radius * radius / (center - origin).lengthSquared());
        auto solid_angle = 2 * pi * (1 - cos_theta_max);
        return  1 / solid_angle;
    }

    Vector3 random(const Vector3& origin) const override {
        Vector3 direction = center - origin;
        auto distance_squared = direction.lengthSquared();
        ONB uvw;
        uvw.buildFromW(direction);
        return uvw.local(randomToSphere(radius, distance_squared));
    }

private:
    static void getSphereUV(const Vector3& p, double& u, double& v) {
        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + pi;
        u = phi / (2*pi);
        v = theta / pi;
    }

    static Vector3 randomToSphere(double radius, double distance_squared) {
        auto r1 = randomDouble();
        auto r2 = randomDouble();
        auto z = 1 + r2*(std::sqrt(1-radius*radius/distance_squared) - 1);

        auto phi = 2*pi*r1;
        auto x = std::cos(phi) * std::sqrt(1-z*z);
        auto y = std::sin(phi) * std::sqrt(1-z*z);

        return Vector3(x, y, z);
    }


    Vector3 center;
    double radius;
    std::shared_ptr<Material> mat;
    AABB bbox;
};

#endif