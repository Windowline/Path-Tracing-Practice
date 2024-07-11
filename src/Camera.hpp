#include "rtweekend.h"
#include "Color.hpp"
#include "Hittable.hpp"
#include "Material.hpp"
#include "PDF.hpp"
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

    double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    int    sqrt_spp;             // Square root of number of samples per pixel
    double recip_sqrt_spp;       // 1 / sqrt_spp

    void render(const Hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                Vector3 pixel_color(0, 0, 0);
//                for (int sample = 0; sample < samples_per_pixel; ++sample) {
//                    Ray r = getRay(i, j);
//                    pixel_color += rayColor(r, max_depth, world);
//                }
                for (int s_j = 0; s_j < sqrt_spp; s_j++) {
                    for (int s_i = 0; s_i < sqrt_spp; s_i++) {
                        Ray r = getRay(i, j, s_i, s_j);
                        pixel_color += rayColor(r, max_depth, world);
                    }
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

        sqrt_spp = int(sqrt(samples_per_pixel));
        pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp);
        recip_sqrt_spp = 1.0 / sqrt_spp;

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

    Vector3 rayColor(const Ray& inputRay, int depth, const Hittable& world) const {
        HitRecord rec;
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return Vector3(0, 0, 0);

        if (!world.hit(inputRay, Interval(0.001, infinity), rec))
            return background;

        // new
        Ray scatteredRay;
        Vector3 attenuation;
        double PDF = 1.0;
        Vector3 colorFromEmission = rec.mat->emitted(inputRay, rec, rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(inputRay, rec, attenuation, scatteredRay, PDF))
            return colorFromEmission;

        CosinePDF surfacePDF(rec.normal);
        scatteredRay = Ray(rec.p, surfacePDF.generate(), inputRay.time());
        PDF = surfacePDF.value(scatteredRay.direction());

        double scatteringPDF = rec.mat->scatteringPDF(inputRay, rec, scatteredRay);

        Vector3 colorFromScatter = (attenuation * scatteringPDF * rayColor(scatteredRay, depth - 1, world)) / PDF;

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

    Ray getRay(int i, int j, int s_i, int s_j) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j for stratified sample square s_i, s_j.
        auto offset = sample_square_stratified(s_i, s_j);

        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }

    Vector3 sample_square_stratified(int s_i, int s_j) const {
        // Returns the vector to a random point in the square sub-pixel specified by grid
        // indices s_i and s_j, for an idealized unit square pixel [-.5,-.5] to [+.5,+.5].

        auto px = ((s_i + randomDouble()) * recip_sqrt_spp) - 0.5;
        auto py = ((s_j + randomDouble()) * recip_sqrt_spp) - 0.5;

        return Vector3(px, py, 0);
    }

    Vector3 pixel_sample_square() const {
        // Returns a random point in the square surrounding a pixel at the origin.
        auto px = -0.5 + randomDouble();
        auto py = -0.5 + randomDouble();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }



    Vector3 pixelSampleSquare() const {
        // Returns a random point in the square surrounding a pixel at the origin.
        auto px = -0.5 + randomDouble();
        auto py = -0.5 + randomDouble();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }
};
