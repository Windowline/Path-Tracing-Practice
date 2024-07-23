#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include "Utils.hpp"
#include "Texture.hpp"
#include "ONB.hpp"
#include "PDF.hpp"

class HitRecord;

class ScatterRecord {
public:
    Vector3 attenuation;
    shared_ptr<PDF> pdf;
    bool isSkipPDF;
    Ray skipPDFRay;
};

class Material {
public:
    virtual bool scatter(const Ray& ray, const HitRecord& rec, ScatterRecord& outSRec) const {
        return false;
    }

    virtual Vector3 emitted(const Ray& ray, const HitRecord& rec, double u, double v, const Vector3& p) const {
        return Vector3(0, 0, 0);
    }

    virtual double scatteringPDF(const Ray& ray, const HitRecord& rec, const Ray& scattered) const {
        return 0;
    }
};


class Lambertian : public Material {
public:
    explicit Lambertian(const Vector3& albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
    Lambertian(shared_ptr<Texture> tex) : tex(tex) {}

    bool scatter(const Ray& ray, const HitRecord& rec, ScatterRecord& outSRec) const {
        outSRec.attenuation = tex->value(rec.u, rec.v, rec.p);
        outSRec.pdf = make_shared<CosinePDF>(rec.normal);
        outSRec.isSkipPDF = false;
        return true;
    }

    double scatteringPDF(const Ray& ray, const HitRecord& rec, const Ray& scattered) const override {
        auto cosTheta = dot(rec.normal, unitVector(scattered.direction()));
        return cosTheta < 0 ? 0 : cosTheta / pi;
    }

private:
    std::shared_ptr<Texture> tex;
};


class Metal : public Material {
public:
    Metal(const Vector3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const Ray& ray, const HitRecord& rec, ScatterRecord& outSRec) const override {
        Vector3 reflected = reflect(ray.direction(), rec.normal);
        reflected = unitVector(reflected) + (fuzz * randomUnitVector());
        outSRec.attenuation = albedo;
        outSRec.pdf = nullptr;
        outSRec.isSkipPDF = true;
        outSRec.skipPDFRay = Ray(rec.p, reflected, ray.time());
        return true;
    }

private:
    Vector3 albedo;
    double fuzz;
};


class Dielectric : public Material {
public:
    Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scatter(const Ray& ray, const HitRecord& rec, ScatterRecord& outSRec) const override {
        outSRec.attenuation = Vector3(1.0, 1.0, 1.0);
        outSRec.pdf = nullptr;
        outSRec.isSkipPDF = true;
        double refractionRatio = rec.isFrontFace ? (1.0 / ir) : ir;

        Vector3 rayDir = unitVector(ray.direction());
        double cosTheta = fmin(dot(-rayDir, rec.normal), 1.0);
        double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

        bool cannotRefract = refractionRatio * sinTheta > 1.0;
        Vector3 scatterDir;

        if (cannotRefract || reflectance(cosTheta, refractionRatio) > randomDouble())
            scatterDir = reflect(rayDir, rec.normal);
        else
            scatterDir = refract(rayDir, rec.normal, refractionRatio);


        outSRec.skipPDFRay = Ray(rec.p, scatterDir, ray.time());

        return true;
    }

private:
    double ir;

    static double reflectance(double cosine, double refIdx) {
        // Schlick's approximation for reflectance.
        auto r0 = (1 - refIdx) / (1 + refIdx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }
};


class Isotropic : public Material {
public:
    Isotropic(const Vector3& albedo) : tex(make_shared<SolidColor>(albedo)) {}
    Isotropic(shared_ptr<Texture> tex) : tex(tex) {}

    bool scatter(const Ray& ray, const HitRecord& rec, ScatterRecord& outSRec) const override {
        outSRec.attenuation = tex->value(rec.u, rec.v, rec.p);
        outSRec.pdf = make_shared<SpherePDF>();
        outSRec.isSkipPDF = false;
        return true;
    }

    double scatteringPDF(const Ray& ray, const HitRecord& rec, const Ray& scattered) const override {
        return 1 / (4 * pi);
    }

private:
    shared_ptr<Texture> tex;
};

class DiffuseLight : public Material {
public:
    DiffuseLight(shared_ptr<Texture> tex) : tex(tex) {}
    DiffuseLight(const Vector3& emit) : tex(make_shared<SolidColor>(emit)) {}

    Vector3 emitted(const Ray& ray, const HitRecord& rec, double u, double v, const Vector3& p) const override {
        if (!rec.isFrontFace)
            return Vector3(0,0,0);
        return tex->value(u, v, p);
    }

private:
    shared_ptr<Texture> tex;
};



#endif