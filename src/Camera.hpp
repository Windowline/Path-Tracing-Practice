#include "Utils.hpp"
#include "Color.hpp"
#include "Hittable.hpp"
#include "Material.hpp"
#include "PDF.hpp"
#include "Quad.hpp"
#include <iostream>

using namespace std;

class Camera {
public:
    double aspectRatio = 1.0;
    int imgWidth = 100;
    int samplePerPixel = 10;
    int maxDepth = 10;
    Vector3 background = Vector3(0, 0, 0);
    bool onSkyBackground = false;

    double fovy = 90;
    Vector3 camPos = Vector3(0, 0, -1);
    Vector3 lookAt = Vector3(0, 0, 0);
    Vector3 up = Vector3(0, 1, 0);

    void render(const Hittable& world, shared_ptr<Quad>& light) {
        initialize();
        std::cout << "P3\n" << imgWidth << ' ' << imgHeight << "\n255\n";
        for (int j = 0; j < imgHeight; ++j) {
            std::clog << "\rScanlines remaining: " << (imgHeight - j) << ' ' << std::flush;
            for (int i = 0; i < imgWidth; ++i) {
                Vector3 pixel_color(0, 0, 0);
                for (int sample = 0; sample < samplePerPixel; ++sample) {
                    Ray r = getRay(i, j);
                    pixel_color += rayColor(r, maxDepth, world, light);
                }
                writeColor(std::cout, pixel_color, samplePerPixel);
            }
        }

        std::clog << "\rDone.                 \n";
    }

private:
    int imgHeight;
    Vector3 pixel00Loc;
    Vector3 pixelDeltaU;
    Vector3 pixelDeltaV;
    Vector3 u, v, w;

    void initialize() {
        imgHeight = static_cast<int>(imgWidth / aspectRatio);
        imgHeight = (imgHeight < 1) ? 1 : imgHeight;

        auto focalLen = (camPos - lookAt).length();
        auto theta = degrees2radians(fovy);
        auto h = tan(theta/2);
        auto viewportHeight = 2 * h * focalLen;
        auto viewportWidth = viewportHeight * (static_cast<double>(imgWidth) / imgHeight);

        w = unitVector(camPos - lookAt);
        u = unitVector(cross(up, w));
        v = cross(w, u);

        Vector3 viewportU = viewportWidth * u;
        Vector3 viewportV = viewportHeight * -v;

        pixelDeltaU = viewportU / imgWidth;
        pixelDeltaV = viewportV / imgHeight;

        auto viewportUpperLeft = camPos - (focalLen * w) - viewportU / 2 - viewportV / 2;
        pixel00Loc = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);
    }

    Vector3 rayColor(const Ray& ray, int depth, const Hittable& world, shared_ptr<Quad>& light) const {
        HitRecord rec;

        if (depth <= 0)
            return Vector3(0, 0, 0);

        if (!world.hit(ray, Interval(0.001, infinity), rec)) {
            if (onSkyBackground) {
                Vector3 rayDir = unitVector(ray.direction());
                auto a = 0.5 * (rayDir.y() + 1.0);
                return (1.0 - a) * Vector3(1.0, 1.0, 1.0) + a * Vector3(0.5, 0.7, 1.0);
            } else {
                return background;
            }
        }

        Ray scattered;
        Vector3 attenuation;
        Vector3 emissionColor = rec.mat->emitted(ray, rec, rec.u, rec.v, rec.p);
        double PDF = 1.0;
        bool isScattered = rec.mat->scatter(ray, rec, attenuation, scattered, PDF);

        if (!isScattered)
            return emissionColor;

        //1. Mix
        auto p0 = make_shared<HittablePDF>(*light.get(), rec.p);
        auto p1 = make_shared<CosinePDF>(rec.normal);
        MixturePDF mixedPDF(p0, p1);
        scattered = Ray(rec.p, mixedPDF.generateRandomVector(), ray.time());
        PDF = mixedPDF.pdfValue(scattered.direction());

        //2. Light Sampling
//        HittablePDF lightPDF(*light.get(), rec.p);
//        scattered = Ray(rec.p, lightPDF.generateRandomVector(), ray.time());
//        PDF = lightPDF.pdfValue(scattered.direction()); // PDF = p(w)

        //3. Cosine Sampling
//        CosinePDF surfacePDF(rec.normal);
//        scattered = Ray(rec.p, surfacePDF.generateRandomVector(), ray.time());
//        PDF = surfacePDF.pdfValue(scattered.direction());

        double scatteringPDF = rec.mat->scatteringPDF(ray, rec, scattered);

        Vector3 sampleColor = rayColor(scattered, depth-1, world, light);
        Vector3 scatterColor = (attenuation * scatteringPDF * sampleColor) / PDF;

        return emissionColor + scatterColor;
    }

    Ray getRay(int i, int j) const {
        auto pixelCenter = pixel00Loc + (i * pixelDeltaU) + (j * pixelDeltaV);
        auto pixelSample = pixelCenter + pixelSampleSquare();
        return Ray(camPos, pixelSample - camPos);
    }

    Vector3 pixelSampleSquare() const {
        auto px = -0.5 + randomDouble();
        auto py = -0.5 + randomDouble();
        return (px * pixelDeltaU) + (py * pixelDeltaV);
    }
};
