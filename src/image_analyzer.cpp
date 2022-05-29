#include "image_analyzer.hpp"


static void emplaceIfNeeded(std::vector<Object> & vec, const Pixel & px) {

}


std::vector<Object> extractObjects(const Image & img) {
    std::vector<Object> objects;

    for (std::uint32_t x = 0; x < img.width(); ++x) {
        for (std::uint32_t y = 0; y < img.height(); ++y) {
            const auto & px = img.at(x, y);
            
            if (not px.isIndexed()) {
                continue;
            }

            emplaceIfNeeded(objects, px);
        }
    }

    return objects;
}

