#ifndef TEXTURE_H
#define TEXTURE_H

#include "Utils.hpp"
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
    CheckerTexture(double scale, std::shared_ptr<SolidColor> even, std::shared_ptr<SolidColor> odd) : invScale(1.0 / scale),
                                                                                                      even(even), odd(odd)
    {}

    CheckerTexture(double scale, const Vector3& c1, const Vector3& c2) : invScale(1.0 / scale),
                                                                         even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2))
    {}

    Vector3 value(double u, double v, const Vector3& p) const {
        auto xInteger = int(std::floor(invScale * p.x()));
        auto yInteger = int(std::floor(invScale * p.y()));
        auto zInteger = int(std::floor(invScale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double invScale;
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
};



class ImageTexture : public Texture {
public:
    ImageTexture(const char* filename) : image(filename) {}

    Vector3 value(double u, double v, const Vector3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (image.height() <= 0)
            return Vector3(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = Interval(0, 1).clamp(u);
        v = 1.0 - Interval(0, 1).clamp(v);  // Flip V to image coordinates

        auto i = int(u * image.width());
        auto j = int(v * image.height());
        auto pixel = image.pixel_data(i,j);

        auto colorScale = 1.0 / 255.0;
        return Vector3(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
    }

private:
    rtw_image image;
};




#endif