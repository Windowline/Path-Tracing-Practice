#ifndef PDF_H
#define PDF_H

#include "rtweekend.h"
#include "OBN.hpp"

class PDF {
public:
    virtual ~PDF() {}

    virtual double value(const Vector3& direction) const = 0;
    virtual Vector3 generate() const = 0;
};

class SpherePDF : public PDF {
public:
    SpherePDF() { }

    double value(const Vector3& direction) const override {
        return 1/ (4 * pi);
    }

    Vector3 generate() const override {
        return randomUnitVector();
    }
};


class CosinePDF : public PDF {
public:
    CosinePDF(const Vector3& w) { uvw.build_from_w(w); }

    double value(const Vector3& direction) const override {
        auto cosineTheta = dot(unit_vector(direction), uvw.w());
        return fmax(0, cosineTheta / pi);
    }

    Vector3 generate() const override {
        return uvw.local(randomUnitVector());
    }

private:
    OBN uvw;
};



#endif