#ifndef RAY_TRACING_ADVANCED_PDF_H
#define RAY_TRACING_ADVANCED_PDF_H
#include "Utils.hpp"
#include "ONB.hpp"

class PDF {
public:
    virtual ~PDF() {}
    virtual double pdfValue(const Vector3& dir) const = 0;
    virtual Vector3 generateRandomVector() const = 0;
};

class SpherePDF : public PDF {
public:
    SpherePDF() { }

    double pdfValue(const Vector3& dir) const override {
        return 1.0 / (4.0 * pi);
    }

    Vector3 generateRandomVector() const override {
        return randomUnitVector();
    }
};


class CosinePDF : public PDF {
public:
    CosinePDF(const Vector3& w) {
        uvw.buildFromW(w);
    }

    double pdfValue(const Vector3& dir) const override {
        auto cosineTheta = dot(unitVector(dir), uvw.w());
        return fmax(0, cosineTheta / pi);
    }

    Vector3 generateRandomVector() const override {
        return uvw.local(randomCosineDirection());
    }

private:
    ONB uvw;
};



class HittablePDF : public PDF {
public:
    HittablePDF(const Hittable& hittableObj, const Vector3& origin)
            : hittableObj(hittableObj), origin(origin)
    {}

    double pdfValue(const Vector3& direction) const override {
        return hittableObj.pdfValue(origin, direction);
    }

    Vector3 generateRandomVector() const override {
        return hittableObj.random(origin);
    }

private:
    const Hittable& hittableObj;
    Vector3 origin;
};

#endif //RAY_TRACING_ADVANCED_PDF_H
