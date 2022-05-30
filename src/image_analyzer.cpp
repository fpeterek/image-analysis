#include "image_analyzer.hpp"

#include <unordered_map>
#include <cmath>


static std::vector<Object> toVector(const std::unordered_map<std::uint32_t, Object> & map) {
    std::vector<Object> vec(map.size(), Object { });

    for (const auto & [idx, obj] : map) {
        vec[idx] = obj;
        vec[idx].id = idx;
    }

    return vec;
}


std::vector<Object> extractObjects(const Image & img) {
    std::unordered_map<std::uint32_t, Object> objects;

    for (std::uint32_t x = 0; x < img.width(); ++x) {
        for (std::uint32_t y = 0; y < img.height(); ++y) {
            const auto & px = img.at(x, y);
            
            if (not px.isIndexed()) {
                continue;
            }

            auto & obj = objects[px.index];
            obj.bounds.leftTop.x = std::min(obj.bounds.leftTop.x, x);
            obj.bounds.leftTop.y = std::min(obj.bounds.leftTop.y, y);
            obj.bounds.rightBottom.x = std::max(obj.bounds.rightBottom.x, x);
            obj.bounds.rightBottom.y = std::max(obj.bounds.rightBottom.y, y);
        }
    }

    return toVector(objects);
}

