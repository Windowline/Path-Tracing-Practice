#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

class Vector3 {
public:
    double e[3];

    Vector3() {
        e[0] = e[1] = e[2] = 0;
    }

    Vector3(double e0, double e1, double e2) {
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
    }

    static Vector3 random() {
        return Vector3(randomDouble(), randomDouble(), randomDouble());
    }

    static Vector3 random(double min, double max) {
        return Vector3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
    }


    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    Vector3 operator-() const { return Vector3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    Vector3& operator+=(const Vector3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vector3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vector3& operator/=(double t) {
        return *this *= 1/t;
    }


    double length() const {
        return sqrt(lengthSquared());
    }

    double lengthSquared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    bool nearZero() const {
        auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }
};


inline std::ostream& operator<<(std::ostream &out, const Vector3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vector3 operator+(const Vector3 &u, const Vector3 &v) {
    return Vector3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vector3 operator-(const Vector3 &u, const Vector3 &v) {
    return Vector3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vector3 operator*(const Vector3 &u, const Vector3 &v) {
    return Vector3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vector3 operator*(double t, const Vector3 &v) {
    return Vector3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vector3 operator*(const Vector3 &v, double t) {
    return t * v;
}

inline Vector3 operator/(Vector3 v, double t) {
    return (1/t) * v;
}

inline double dot(const Vector3 &u, const Vector3 &v) {
    return u.e[0] * v.e[0]
           + u.e[1] * v.e[1]
           + u.e[2] * v.e[2];
}

inline Vector3 cross(const Vector3 &u, const Vector3 &v) {
    return Vector3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vector3 unitVector(Vector3 v) {
    return v / v.length();
}

inline Vector3 randomUnitSphere() {
    while (true) {
        auto p = Vector3::random(-1, 1);
        if (p.lengthSquared() < 1)
            return p;
    }
}

inline Vector3 randomUnitVector() {
    return unitVector(randomUnitSphere());
}

inline Vector3 randomCosineDirection() {
    auto r1 = randomDouble();
    auto r2 = randomDouble();

    auto phi = 2 * pi * r1;
    auto x = cos(phi) * sqrt(r2);
    auto y = sin(phi) * sqrt(r2);
    auto z = sqrt(1 - r2);

    return Vector3(x, y, z);
}

inline Vector3 randomHemisphere(const Vector3& normal) {
    Vector3 onUnitSphere = randomUnitVector();
    if (dot(onUnitSphere, normal) > 0.0) // In the same hemisphere as the normal
        return onUnitSphere;
    else
        return -onUnitSphere;
}

Vector3 reflect(const Vector3& v, const Vector3& n) {
    return v - 2 * dot(v,n) * n;
}

inline Vector3 refract(const Vector3& uv, const Vector3& n, double etaiOverEtat) {
    auto cosTheta = fmin(dot(-uv, n), 1.0);
    Vector3 rOutPerp = etaiOverEtat * (uv + cosTheta * n);
    Vector3 rOutParallel = -sqrt(fabs(1.0 - rOutPerp.lengthSquared())) * n;
    return rOutPerp + rOutParallel;
}

#endif