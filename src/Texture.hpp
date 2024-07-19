#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"
#include "STBImageLoader.hpp"

class Texture {
public:
    virtual Vector3 value(double u, double v, const Vector3& p) const = 0;
};

class SolidColor : public Texture {
public:
    SolidColor(const Vector3& albedo) : albedo(albedo) {}

    SolidColor(double red, double green, double blue) : albedo(Vector3(red, green, blue)) {}

    Vector3 value(double u, double v, const Vector3& p) const {
        return albedo;
    }

private:
    Vector3 albedo;
};

class CheckerTexture : public Texture {
public:
    CheckerTexture(double scale, std::shared_ptr<SolidColor> even, std::shared_ptr<SolidColor> odd) : inv_scale(1.0 / scale),
                                                                                                      even(even), odd(odd)
    {}

    CheckerTexture(double scale, const Vector3& c1, const Vector3& c2) : inv_scale(1.0 / scale),
                                                                         even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2))
    {}

    Vector3 value(double u, double v, const Vector3& p) const {
        auto xInteger = int(std::floor(inv_scale * p.x()));
        auto yInteger = int(std::floor(inv_scale * p.y()));
        auto zInteger = int(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
};



class ImageTexture : public Texture {
public:
    ImageTexture(const char* filename) : image(filename) {}

    Vector3 value(double u, double v, const Vector3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (image.height() <= 0) return Vector3(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = Interval(0, 1).clamp(u);
        v = 1.0 - Interval(0, 1).clamp(v);  // Flip V to image coordinates

        auto i = int(u * image.width());
        auto j = int(v * image.height());
        auto pixel = image.pixel_data(i,j);

        auto color_scale = 1.0 / 255.0;
        return Vector3(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    rtw_image image;
};




#endif