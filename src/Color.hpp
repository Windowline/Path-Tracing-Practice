#ifndef COLOR_H
#define COLOR_H

#include "Vector3.hpp"
#include "Interval.hpp"

#include <iostream>


inline double linear2gamma(double linear) {
    return sqrt(linear);
}


void writeColor(std::ostream &out, Vector3 pixelColor, int samplesPerPixel) {
    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    // Divide the color by the number of samples.
    auto scale = 1.0 / samplesPerPixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Apply the linear to gamma transform.
    r = linear2gamma(r);
    g = linear2gamma(g);
    b = linear2gamma(b);

    // Write the translated [0,255] value of each color component.
    static const Interval intensity(0.000, 0.999);
    out << static_cast<int>(256 * intensity.clamp(r)) << ' '
        << static_cast<int>(256 * intensity.clamp(g)) << ' '
        << static_cast<int>(256 * intensity.clamp(b)) << '\n';
}

#endif