#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.h"

class texture {
public:
//    virtual ~texture() = default;

    virtual vec3 value(double u, double v, const vec3& p) const = 0;
};

class solid_color : public texture {
public:
    solid_color(const vec3& albedo) : albedo(albedo) {}

    solid_color(double red, double green, double blue) : albedo(vec3(red,green,blue)) {}

    vec3 value(double u, double v, const vec3& p) const {
        return albedo;
    }

private:
    vec3 albedo;
};

class checker_texture : public texture {
public:
    checker_texture(double scale, std::shared_ptr<solid_color> even, std::shared_ptr<solid_color> odd) : inv_scale(1.0 / scale),
        even(even), odd(odd)
    {}

    checker_texture(double scale, const vec3& c1, const vec3& c2) : inv_scale(1.0 / scale),
        even(std::make_shared<solid_color>(c1)), odd(std::make_shared<solid_color>(c2))
    {}

    vec3 value(double u, double v, const vec3& p) const {
        auto xInteger = int(std::floor(inv_scale * p.x()));
        auto yInteger = int(std::floor(inv_scale * p.y()));
        auto zInteger = int(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    std::shared_ptr<texture> even;
    std::shared_ptr<texture> odd;
};

#endif