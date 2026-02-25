#pragma once

#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>

// A node in a bounding volume hierarchy
class bvh_node : public hittable
{
public:
    bvh_node(hittable_list list)
        : bvh_node(list.objects, 0, list.objects.size())
    {
        // The input hittable list is copied on purpose
    }

    bvh_node(std::vector<std::shared_ptr<hittable>>& objects, size_t start, size_t end)
    {
        // Build the bounding box of the span of source objects
        bbox = aabb::empty;
        for (size_t object_index = start; object_index < end; ++object_index)
        {
            bbox = aabb(bbox, objects[object_index]->bounding_box());
        }
        const auto axis = bbox.longest_axis();
        auto comparator = (axis == 0) ? box_x_compare :
                          (axis == 1) ? box_y_compare : box_z_compare;
        size_t object_span = end - start; // Open-ended interval

        if (object_span == 1)
        {
            // To avoid nullptr checks we duplicate single elements
            left = objects[start];
            right = objects[start];
        }
        else if (object_span == 2)
        {
            left = objects[start];
            right = objects[start + 1];
        }
        else
        {
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);
            const auto mid = start + object_span / 2;
            left = std::make_shared<bvh_node>(objects, start, mid); // Open-ended interval
            right = std::make_shared<bvh_node>(objects, mid, end);
        }

        bbox = aabb(left->bounding_box(), right->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
        if (!bbox.hit(r, ray_t))
        {
            return false;
        }

        bool hit_left = left->hit(r, ray_t, rec);
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const override { return bbox; }

    static bool box_compare(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b, int axis_index)
    {
        const auto a_axis_int = a->bounding_box().axis_interval(axis_index);
        const auto b_axis_int = b->bounding_box().axis_interval(axis_index);
        return a_axis_int.min < b_axis_int.min;
    }

    static bool box_x_compare(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b) { return box_compare(a, b, 0); }
    static bool box_y_compare(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b) { return box_compare(a, b, 1); }
    static bool box_z_compare(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b) { return box_compare(a, b, 2); }

private:
    std::shared_ptr<hittable> left;
    std::shared_ptr<hittable> right;
    aabb bbox;
};