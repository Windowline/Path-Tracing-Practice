#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include "rtweekend.h"
#include "Texture.hpp"

class HitRecord;

class Material {
public:
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const {
        return false;
    }

    virtual Vector3 emitted(double u, double v, const Vector3& p) const {
        return Vector3(0, 0, 0);
    }
};


class Lambertian : public Material {
public:
    Lambertian(const Vector3& albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
    Lambertian(std::shared_ptr<Texture> tex) : tex(tex) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const override {
        auto scatter_direction = rec.normal + randomUnitVector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = Ray(rec.p, scatter_direction);
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    std::shared_ptr<Texture> tex;
};


class Metal : public Material {
public:
    Metal(const Vector3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const override {
        Vector3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * randomUnitVector());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    Vector3 albedo;
    double fuzz;
};


class Dielectric : public Material {
public:
    Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scattered) const override {
        attenuation = Vector3(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

        Vector3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vector3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > randomDouble())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = Ray(rec.p, direction);
        return true;
    }

private:
    double ir; // Index of Refraction

    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }
};

class DiffuseLight : public Material {
public:
    DiffuseLight(shared_ptr<Texture> tex) : tex(tex) {}
    DiffuseLight(const Vector3& emit) : tex(make_shared<SolidColor>(emit)) {}

    Vector3 emitted(double u, double v, const Vector3& p) const override {
        return tex->value(u, v, p);
    }

private:
    shared_ptr<Texture> tex;
};

#endif