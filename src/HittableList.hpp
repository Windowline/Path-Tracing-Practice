#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "Hittable.hpp"

#include <memory>
#include <vector>

class HittableList : public Hittable {
public:
    std::vector< std::shared_ptr<Hittable> > objects;

    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(std::shared_ptr<Hittable> object) {
        objects.push_back(object);
        bbox = AABB(bbox, object->boundingBox());
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& outRec) const override {
        HitRecord tmpRec;
        bool hitAnything = false;
        auto closest = ray_t.max;

        for (const auto& object : objects) {
            if (object->hit(r, Interval(ray_t.min, closest), tmpRec)) {
                hitAnything = true;
                closest = tmpRec.t;
                outRec = tmpRec;
            }
        }

        return hitAnything;
    }

    AABB boundingBox() const override { return bbox; }


private:
    AABB bbox;

};

#endif