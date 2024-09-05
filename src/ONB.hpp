#ifndef ONB_H
#define ONB_H

#include "Utils.hpp"
#include "Vector3.hpp"

class ONB {
public:
    ONB() {}

    Vector3 operator[](int i) const { return axis[i]; }
    Vector3& operator[](int i) { return axis[i]; }

    Vector3 u() const { return axis[0]; }
    Vector3 v() const { return axis[1]; }
    Vector3 w() const { return axis[2]; }

    Vector3 local(double a, double b, double c) const {
        return a * u() + b * v() + c * w();
    }

    Vector3 local(const Vector3& a) const {
        return a.x() * u() + a.y ()* v() + a.z () * w();
    }

    void buildFromW(const Vector3& w) {
        Vector3 unitW = unitVector(w);
        Vector3 a = (fabs(unitW.x()) > 0.9) ? Vector3(0, 1, 0) : Vector3(1, 0, 0);
        Vector3 v = unitVector(cross(unitW, a));
        Vector3 u = cross(unitW, v);
        axis[0] = u;
        axis[1] = v;
        axis[2] = unitW;
    }

public:
    Vector3 axis[3];
};


#endif