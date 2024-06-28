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

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        HitRecord temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max; //이해.

        for (const auto& object : objects) {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    AABB boundingBox() const override { return bbox; }


private:
    AABB bbox;

};

#endif