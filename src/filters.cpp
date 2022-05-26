#include "filters.hpp"

#include <unordered_set>

#include "signals.hpp"


static void reindex(Image & img) {

    std::uint32_t idx = 0;
    std::unordered_map<std::uint32_t, std::uint32_t> indexMap;

    for (uint64_t x = 0; x < img.width(); ++x) {
        for (uint64_t y = 0; y < img.height(); ++y) {
            auto & pixel = img.at(x, y);

            if (pixel.isIndexed()) {
                if (not indexMap.count(pixel.index)) {
                    indexMap[pixel.index] = idx++;
                }

                pixel.index = indexMap[pixel.index];
            }
        }
    }
}


Image filterBySize(const Image & input, const int threshold) {

    Image dest(input);

    const auto perimeters = signals::getPerimeters(input);

    for (uint64_t x = 0; x < dest.width(); ++x) {
        for (uint64_t y = 0; y < dest.height(); ++y) {
            auto & pixel = dest.at(x, y);
            if (pixel.isIndexed() and perimeters.at(pixel.index) < threshold) {
                pixel.index = Pixel::noIndex;
                pixel.color = 0;
            }
        }
    }

    reindex(dest);

    return dest;
}

