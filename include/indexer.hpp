#ifndef IMAGE_ANALYSIS_INDEXER_HPP
#define IMAGE_ANALYSIS_INDEXER_HPP

#include <functional>

#include "image.hpp"


class Indexer {

    Image dest;
    uint32_t idxCounter = 0;

    void assignRecursively(uint32_t x, uint32_t y);
    void assignIndex(uint32_t x, uint32_t y);
    void assignIndices();


public:

    Image assignIndices(const Image & img);

};

#endif

