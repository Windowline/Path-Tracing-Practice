#ifndef BVH_H
#define BVH_H

#include "Utils.hpp"

#include "AABB.hpp"
#include "Hittable.hpp"
#include "HittableList.hpp"

class BVHNode : public Hittable {
public:
    BVHNode(HittableList list) {
        build(list.objects, 0, list.objects.size());
    }

    BVHNode(std::vector< std::shared_ptr<Hittable> >& objects, size_t start, size_t end) {
        build(objects, start, end);
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        if (!bbox.hit(r, ray_t))
            return false;

        bool hit_left = left->hit(r, ray_t, rec);
        bool hit_right = right->hit(r, Interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    AABB boundingBox() const override {
        return bbox;
    }

private:
    void build(std::vector< std::shared_ptr<Hittable> >& objects, size_t start, size_t end) {
        bbox = AABB::empty;
        for (size_t object_index=start; object_index < end; object_index++)
            bbox = AABB(bbox, objects[object_index]->boundingBox());

        int axis = bbox.longestAxis();

        auto comparator = (axis == 0) ? boxCompareX
                                      : (axis == 1) ? boxCompareY
                                                    : boxCompareZ;

        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = objects[start];
        } else if (object_span == 2) {
            left = objects[start];
            right = objects[start+1];
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto mid = start + object_span/2;
            left = make_shared<BVHNode>(objects, start, mid);
            right = make_shared<BVHNode>(objects, mid, end);
        }

        bbox = AABB(left->boundingBox(), right->boundingBox());
    }

    static bool boxCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis_index) {
        auto a_axis_interval = a->boundingBox().axis_interval(axis_index);
        auto b_axis_interval = b->boundingBox().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool boxCompareX(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return boxCompare(a, b, 0);
    }

    static bool boxCompareY(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return boxCompare(a, b, 1);
    }

    static bool boxCompareZ(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return boxCompare(a, b, 2);
    }

    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    AABB bbox;
};

#endif