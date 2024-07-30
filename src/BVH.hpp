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

    bool hit(const Ray& ray, Interval interval, HitRecord& rec) const override {
        if (!bbox.hit(ray, interval))
            return false;

        bool hitLeft = left->hit(ray, interval, rec);
        bool hitRight = right->hit(ray, Interval(interval.min, hitLeft ? rec.t : interval.max), rec);

        return hitLeft || hitRight;
    }

    AABB boundingBox() const override {
        return bbox;
    }

private:
    void build(std::vector< std::shared_ptr<Hittable> >& objects, size_t start, size_t end) {
        bbox = AABB::empty;
        for (size_t index=start; index < end; index++)
            bbox = AABB(bbox, objects[index]->boundingBox());

        int axis = bbox.longestAxis();

        auto comparator = (axis == 0) ? boxCompareX
                                      : (axis == 1) ? boxCompareY
                                                    : boxCompareZ;

        size_t span = end - start;

        if (span == 1) {
            left = right = objects[start];
        } else if (span == 2) {
            left = objects[start];
            right = objects[start+1];
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto mid = start + span / 2;
            left = make_shared<BVHNode>(objects, start, mid);
            right = make_shared<BVHNode>(objects, mid, end);
        }

        bbox = AABB(left->boundingBox(), right->boundingBox());
    }

    static bool boxCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis_index) {
        auto axisIntervalA = a->boundingBox().axisInterval(axis_index);
        auto axisIntervalB = b->boundingBox().axisInterval(axis_index);
        return axisIntervalA.min < axisIntervalB.min;
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