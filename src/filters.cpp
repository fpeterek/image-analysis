#include "filters.hpp"

#include "signals.hpp"


Image filterBySize(const Image & input, const int threshold) {

    Image dest(input);

    const auto perimeters = signals::getPerimeters(input);

    for (uint64_t x = 0; x < dest.width(); ++x) {
        for (uint64_t y = 0; y < dest.height(); ++y) {
            auto & pixel = dest.at(x, y);
            if (pixel.isIndexed() and perimeters.at(pixel.index) < threshold) {
                pixel.index = 0;
                pixel.color = 0;
            }
        }
    }
    return dest;
}

