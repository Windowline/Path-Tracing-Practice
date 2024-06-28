#ifndef RAY_H
#define RAY_H

#include "Vector3.hpp"

class Ray {
public:
    Ray() {}

    Ray(const Vector3& origin, const Vector3& direction)
            : orig(origin), dir(direction), tm(0) {}

    Ray(const Vector3& origin, const Vector3& direction, double time)
            : orig(origin), dir(direction), tm(time) {}

    Vector3 origin() const  { return orig; }
    Vector3 direction() const { return dir; }

    Vector3 at(double t) const {
        return orig + t*dir;
    }

    double time() const { return tm; }

private:
    Vector3 orig;
    Vector3 dir;
    double tm;
};

#endif