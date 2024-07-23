#ifndef RAY_TRACING_ADVANCED_PDF_H
#define RAY_TRACING_ADVANCED_PDF_H
#include "Utils.hpp"
#include "ONB.hpp"

class PDF {
public:
    virtual ~PDF() {}
    virtual double value(const Vector3& dir) const = 0;
    virtual Vector3 generate() const = 0;
};

class SpherePDF : public PDF {
public:
    SpherePDF() { }

    double value(const Vector3& dir) const override {
        return 1.0 / (4.0 * pi);
    }

    Vector3 generate() const override {
        return randomUnitVector();
    }
};


class cosinePDF : public PDF {
public:
    cosinePDF(const Vector3& w) {
        uvw.buildFromW(w);
    }

    double value(const Vector3& dir) const override {
        auto cosineTheta = dot(unitVector(dir), uvw.w());
        return fmax(0, cosineTheta / pi);
    }

    Vector3 generate() const override {
        return uvw.local(randomCosineDirection());
    }

private:
    onb uvw;
};

#endif //RAY_TRACING_ADVANCED_PDF_H
