#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.h"
#include "rtw_stb_image.h"

class texture {
public:
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



class image_texture : public texture {
public:
    image_texture(const char* filename) : image(filename) {}

    vec3 value(double u, double v, const vec3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (image.height() <= 0) return vec3(0,1,1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = interval(0,1).clamp(u);
        v = 1.0 - interval(0,1).clamp(v);  // Flip V to image coordinates

        auto i = int(u * image.width());
        auto j = int(v * image.height());
        auto pixel = image.pixel_data(i,j);

        auto color_scale = 1.0 / 255.0;
        return vec3(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }

private:
    rtw_image image;
};




#endif