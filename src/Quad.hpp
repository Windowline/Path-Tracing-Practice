#ifndef QUAD_H
#define QUAD_H

#include "utils.h"

#include "Hittable.hpp"

class Quad : public Hittable {
public:
    Quad(const Vector3& Q, const Vector3& u, const Vector3& v, shared_ptr<Material> mat)
            : Q(Q), u(u), v(v), mat(mat)
    {
        auto n = cross(u, v);
        normal = unitVector(n);
        D = dot(normal, Q);
        w = n / dot(n,n);

        set_bounding_box();
    }

    virtual void set_bounding_box() {
        // Compute the bounding box of all four vertices.
        auto bbox_diagonal1 = AABB(Q, Q + u + v);
        auto bbox_diagonal2 = AABB(Q + u, Q + v);
        bbox = AABB(bbox_diagonal1, bbox_diagonal2);
    }

    AABB boundingBox() const override { return bbox; }

    bool hit(const Ray& r, Interval ray_t, HitRecord& outRec) const override {
        auto denom = dot(normal, r.direction());

        if (fabs(denom) < 1e-8)
            return false;

        auto t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
            return false;

        auto intersection = r.at(t);
        Vector3 planar_hitpt_vector = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));

        if (!isInterior(alpha, beta, outRec))
            return false;

        outRec.t = t;
        outRec.p = intersection;
        outRec.mat = mat;
        outRec.setFaceNormal(r, normal);

        return true;
    }

    virtual bool isInterior(double a, double b, HitRecord& outRec) const {
        Interval unit_interval = Interval(0, 1);

        if (!unit_interval.contains(a) || !unit_interval.contains(b))
            return false;

        outRec.u = a;
        outRec.v = b;
        return true;
    }


private:
    Vector3 Q;
    Vector3 u, v;
    Vector3 w;
    shared_ptr<Material> mat;
    AABB bbox;
    Vector3 normal;
    double D;
};

#endif