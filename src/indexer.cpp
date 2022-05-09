#include "indexer.hpp"

void Indexer::assignRecursively(const uint32_t x, const uint32_t y) {

    auto & px = dest.at(x, y);
    if (px.color != Pixel::colorMax or px.isIndexed()) {
        return;
    }

    px.index = idxCounter;


    if (x) {
        assignRecursively(x-1, y);
    }
    if (x < dest.width()-1) {
        assignRecursively(x+1, y);
    }
    if (y) {
        assignRecursively(x, y-1);
    }
    if (y < dest.height()-1) {
        assignRecursively(x, y+1);
    }

}

void Indexer::assignIndex(const uint32_t x, const uint32_t y) {

    auto & px = dest.at(x, y);
    
    if (px.color != Pixel::colorMax or px.isIndexed()) {
        return;
    }

    assignRecursively(x, y);

    ++idxCounter;
}

void Indexer::assignIndices() {
    for (uint32_t y = 0; y < dest.height(); ++y) {
        for (uint32_t x = 0; x < dest.width(); ++x) {
            assignIndex(x, y);
        }
    }
}

Image Indexer::assignIndices(const Image & img) {

    dest = Image(img);
    idxCounter = 0;

    assignIndices();

    return dest;
}

