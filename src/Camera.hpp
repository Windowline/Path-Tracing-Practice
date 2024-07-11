#include "rtweekend.h"
#include "Color.hpp"
#include "Hittable.hpp"
#include "Material.hpp"

#include <iostream>

class Camera {
public:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width  = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth         = 10;   // Maximum number of ray bounces into scene
    Vector3  background = Vector3(0, 0, 0);               // Scene background color

    double vfov   = 90;              // Vertical view angle (field of view)
    Vector3 lookfrom = Vector3(0, 0, -1);  // Point camera is looking from
    Vector3 lookat   = Vector3(0, 0, 0);   // Point camera is looking at
    Vector3 vup    = Vector3(0, 1, 0);     // Camera-relative "up" direction

    void render(const Hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                Vector3 pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    Ray r = getRay(i, j);
                    pixel_color += rayColor(r, max_depth, world);
                }
                writeColor(std::cout, pixel_color, samples_per_pixel);
            }
        }

        std::clog << "\rDone.                 \n";
    }

private:
    int    image_height;   // Rendered image height
    Vector3   center;         // Camera center
    Vector3   pixel00_loc;    // Location of pixel 0, 0
    Vector3   pixel_delta_u;  // Offset to pixel to the right
    Vector3   pixel_delta_v;  // Offset to pixel below
    Vector3   u, v, w;        // Camera frame basis vectors

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        center = lookfrom;

        // Determine viewport dimensions.
        auto focal_length = (lookfrom - lookat).length();
        auto theta = degrees2radians(vfov);
        auto h = tan(theta/2);
        auto viewport_height = 2 * h * focal_length;
        auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        Vector3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        Vector3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width; // 200, 100 --> 2,  || 50, 100 --> 0.5
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focal_length * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    Vector3 rayColor(const Ray& r, int depth, const Hittable& world) const {
        HitRecord rec;

        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return Vector3(0, 0, 0);

        if (!world.hit(r, Interval(0.001, infinity), rec)) {
            Vector3 unit_direction = unit_vector(r.direction());
            auto a = 0.5*(unit_direction.y() + 1.0);
            return (1.0 - a) * Vector3(1.0, 1.0, 1.0) + a * Vector3(0.5, 0.7, 1.0);
        }

        // new
        Ray scattered;
        Vector3 attenuation;
        double PDF = 1.0;
        bool isScattered = rec.mat->scatter(r, rec, attenuation, scattered, PDF);
        Vector3 colorFromEmission(0, 0, 0);

        if (!isScattered)
            return colorFromEmission; // color from emission

        double scatteringPDF = rec.mat->scatteringPDF(r, rec, scattered);
        assert(PDF >= 0.0 && PDF <= 1.0);
        assert(scatteringPDF >= 0.0 && scatteringPDF <= 1.0);

        Vector3 colorFromScatter = (attenuation * 1.0 * rayColor(scattered, depth - 1, world)) / 1.0;
//        Vector3 colorFromScatter = (attenuation * scatteringPDF * rayColor(scattered, depth - 1, world)) / PDF;

        return colorFromEmission + colorFromScatter;
    }



    Ray getRay(int i, int j) const {
        // Get a randomly sampled camera ray for the pixel at location i,j.

        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixelSampleSquare();

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }

    Vector3 pixelSampleSquare() const {
        // Returns a random point in the square surrounding a pixel at the origin.
        auto px = -0.5 + randomDouble();
        auto py = -0.5 + randomDouble();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }
};
