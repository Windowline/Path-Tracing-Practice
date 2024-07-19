#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"
#include "Interval.hpp"

class AABB {
public:
    Interval x, y, z;

    AABB() {}

    AABB(const Interval& x, const Interval& y, const Interval& z)
            : x(x), y(y), z(z) {
        padMinimums();
    }

    AABB(const Vector3& a, const Vector3& b) {
        x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
        padMinimums();
    }

    AABB(const AABB& box0, const AABB& box1) {
        x = Interval(box0.x, box1.x);
        y = Interval(box0.y, box1.y);
        z = Interval(box0.z, box1.z);
    }

    const Interval& axis_interval(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit(const Ray& r, Interval ray_t) const {
        const Vector3& ray_orig = r.origin();
        const Vector3&   ray_dir  = r.direction();

        for (int axis = 0; axis < 3; axis++) {
            const Interval& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis];

            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            if (t0 < t1) {
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            } else {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 < ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }

    int longestAxis() const {
        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }

    static const AABB empty, universe;

private:
    void padMinimums() {
        double delta = 0.0001;
        if (x.size() < delta) x = x.expand(delta);
        if (y.size() < delta) y = y.expand(delta);
        if (z.size() < delta) z = z.expand(delta);
    }
};

const Interval intervalEmpty   (+std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
const Interval intervalUniverse(-std::numeric_limits<double>::infinity(), +std::numeric_limits<double>::infinity());

const AABB AABB::empty = AABB(intervalEmpty, intervalEmpty, intervalEmpty);
const AABB AABB::universe = AABB(intervalUniverse, intervalUniverse, intervalUniverse);

AABB operator+(const AABB& bbox, const Vector3& offset) {
    return AABB(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

AABB operator+(const Vector3& offset, const AABB& bbox) {
    return bbox + offset;
}

#endif