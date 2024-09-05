#ifndef STB_IMAGE_LOADER_HPP
#define STB_IMAGE_LOADER_HPP

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
#pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "external/stb_image.h"

#include <cstdlib>
#include <iostream>

class rtw_image {
public:
    rtw_image() {}

    rtw_image(const char* image_filename) {

        auto filename = std::string(image_filename);
        auto imagedir = getenv("RTW_IMAGES");

        // Hunt for the image file in some likely locations.
        if (imagedir && load(std::string(imagedir) + "/" + image_filename)) return;
        if (load(filename)) return;
        if (load("images/" + filename)) return;
        if (load("../images/" + filename)) return;
        if (load("../../images/" + filename)) return;
        if (load("../../../images/" + filename)) return;
        if (load("../../../../images/" + filename)) return;
        if (load("../../../../../images/" + filename)) return;
        if (load("../../../../../../images/" + filename)) return;

        std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
    }

    ~rtw_image() {
        delete[] bdata;
        STBI_FREE(fdata);
    }

    bool load(const std::string& filename) {
        // Loads the linear (gamma=1) image data from the given file name. Returns true if the
        // load succeeded. The resulting data buffer contains the three [0.0, 1.0]
        // floating-point values for the first pixel (red, then green, then blue). Pixels are
        // contiguous, going left to right for the width of the image, followed by the next row
        // below, for the full height of the image.

        auto n = bytesPerPixel; // Dummy out parameter: original components per pixel
        fdata = stbi_loadf(filename.c_str(), &imgW, &imgH, &n, bytesPerPixel);
        if (fdata == nullptr) return false;

        bytesPerScanline = imgW * bytesPerPixel;
        convertToBytes();
        return true;
    }

    int width()  const { return (fdata == nullptr) ? 0 : imgW; }
    int height() const { return (fdata == nullptr) ? 0 : imgH; }

    const unsigned char* pixel_data(int x, int y) const {
        // Return the address of the three RGB bytes of the pixel at x,y. If there is no image
        // data, returns magenta.
        static unsigned char magenta[] = { 255, 0, 255 };
        if (bdata == nullptr) return magenta;

        x = clamp(x, 0, imgW);
        y = clamp(y, 0, imgH);

        return bdata + y * bytesPerScanline + x * bytesPerPixel;
    }

private:
    const int      bytesPerPixel = 3;
    float         *fdata = nullptr;         // Linear floating point pixel data
    unsigned char *bdata = nullptr;         // Linear 8-bit pixel data
    int            imgW = 0;         // Loaded image width
    int            imgH = 0;        // Loaded image height
    int            bytesPerScanline = 0;

    static int clamp(int x, int low, int high) {
        // Return the value clamped to the range [low, high).
        if (x < low) return low;
        if (x < high) return x;
        return high - 1;
    }

    static unsigned char float_to_byte(float value) {
        if (value <= 0.0)
            return 0;
        if (1.0 <= value)
            return 255;
        return static_cast< unsigned char >(256.0 * value);
    }

    void convertToBytes() {
        // Convert the linear floating point pixel data to bytes, storing the resulting byte
        // data in the `bdata` member.

        int total_bytes = imgW * imgH * bytesPerPixel;
        bdata = new unsigned char[total_bytes];

        // Iterate through all pixel components, converting from [0.0, 1.0] float values to
        // unsigned [0, 255] byte values.

        auto *bptr = bdata;
        auto *fptr = fdata;
        for (auto i=0; i < total_bytes; i++, fptr++, bptr++)
            *bptr = float_to_byte(*fptr);
    }
};

// Restore MSVC compiler warnings
#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif